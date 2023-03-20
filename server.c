#include "serverNetwork.h"
#include <termio.h>

void launch();
void *console(void *package)
{
	WRAP *p = (WRAP*)package;

	while(getchar()!='q');

	pthread_mutex_lock(&p->lock);
	p->end = 1;
	if(p->number==p->maxNumber)
	{
		pthread_cond_signal(&p->signal);
	}
	pthread_mutex_unlock(&p->lock);
	return NULL;
}
int main()
{
	launch();
}

void launch()
{
	printf("Please input the port you want:");
	int port = 0;
	scanf("%d",&port);
	printf("please input the max number of the connection:");
	int listenNumber = 0;
	scanf("%d",&listenNumber);
	int s_sock = initialServerNetwork(port,listenNumber);
	char buffer[2048]={0};
	int recvResult = 0;
	if(s_sock==0)
	{
		printf("initial network fail\n");
		return ;
	}
	WRAP * package = initialWrap(s_sock,listenNumber);
	if(package == NULL)
	{
		printf("initial WRAP fail\n");
		shutdown(s_sock,SHUT_RDWR);
		return ;
	}
	pthread_t thread1,thread2;
	if(-1 == pthread_create(&thread1,NULL,listenClient,(void*)package))
	{
		printf("create thread faild\n");
		shutdown(s_sock,SHUT_RDWR);
		destroyWrap(package);
	}
	if(-1 == pthread_create(&thread2,NULL,console,(void*)package))
	{
		printf("create thread 2 fail\n");
		shutdown(s_sock,SHUT_RDWR);
		destroyWrap(package);
	}
	while(package != NULL)
	{
		pthread_mutex_lock(&package->lock);
		if(package->end)
		{
			pthread_mutex_unlock(&package->lock);
			pthread_cancel(thread2);
			pthread_join(thread2,NULL);
			destroyWrap(package);
			package = NULL;
			break;
		}
		pthread_mutex_unlock(&package->lock);
		for(NODE *p = package->clientTree;p!= NULL;p=p->next)
		{
			if((recvResult = recv(p->id,buffer,2048,MSG_DONTWAIT))==-1&&errno!=EAGAIN)
			{
				printf("the %s:%d recv fail\n",p->ip,p->port);
				pthread_mutex_lock(&package->lock);
				p=deleteNode(&package->clientTree,p->id);
				pthread_mutex_unlock(&package->lock);
				if(p==NULL)
				{
					break;
				}
				continue;
			}
			else if(recvResult == 0)
			{
				printf("the %s:%d close the socket\n",p->ip,p->port);
				pthread_mutex_lock(&package->lock);
				p=deleteNode(&package->clientTree,p->id);
				pthread_mutex_unlock(&package->lock);
				if(p==NULL)
				{
					break;
				}
				continue;
			}
			else if(recvResult >0)
			{
				transferMessage(package,p->id,buffer);

			}

		}
		
	}
	return ;
}
