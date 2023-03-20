#include "serverNetwork.h"

int initialServerNetwork(int port,int listenNumber)
{
	if(port<=0&&port>25565)
	{
		printf("your port is illegal\n");
		return 0;
	}
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		printf("socket fail\n");
		return 0;
	}
	struct sockaddr_in saddr;
	bzero(&saddr,sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_port = htons(port);
	if(-1==bind(sock,(struct sockaddr*)&saddr,sizeof(struct sockaddr)))
	{
		printf("bind fail\n");
		shutdown(sock,SHUT_RDWR);
		return 0;
	}
	if(-1==listen(sock,listenNumber))
	{
		printf("listen fail\n");
		shutdown(sock,SHUT_RDWR);
		return 0;
	}
	return sock;
}
//将第一个空格以前的字符串进行拷贝
static int splitString(char *dest,char *source)
{
	int length = 0;
	if(sizeof(dest)>sizeof(source))
	{
		length = sizeof(source);
	}
	else if(sizeof(dest)<=sizeof(source))
	{
		length = sizeof(dest);
	}
	length--;
	for(int i=0;i<length;i++)
	{
		if(source[i] <32)
		{
			dest[i] = '\0';
			return 1;
		}
		dest[i] = source[i];
	}
	return 0;
}
//将信息发送给其他客户端
void transferMessage(WRAP *package,int cfd,char *message)
{
	NODE *p = package->clientTree;
	int messageLen = strlen(message);
	pthread_mutex_lock(&package->lock);
	while(p!=NULL)
	{
		if(p->id == cfd)
		{
			p = p->next;
			continue;
		}
		if( send(p->id,message,messageLen,0) <= 0)
		{
			printf("send message %s:%d fail\n",p->ip,p->port);
			printf("now close the connection with it\n");
			shutdown(p->id,SHUT_RDWR);
			if(package->number == package->maxNumber)
			{
				pthread_cond_signal(&package->signal);
			}
			deleteNode(&package->clientTree,p->id);
			package->number--;
		}
		p = p->next;
	}
	pthread_mutex_unlock(&package->lock);
	return;
}


//等待其他客户端的连接
void *listenClient(void *p)
{
	WRAP *package = (WRAP *)p;
	int flag = 0;				//记录是否recv正常
	char buffer[1024] = {0};	//用于接收客户端发送的信息
	char tempBuffer[1024]={0};  //用于解析客户端发送的注册信息
	struct sockaddr temp ;
	struct sockaddr_in *temp1;
	char * tempIP = NULL;
	unsigned int tempPort = 0;
	unsigned int addrLen = sizeof(struct sockaddr);
	int cfd = 0;
	time_t t1,t2;
	t1 = time(&t2);
	bzero(&temp,addrLen);

	pthread_mutex_lock(&package->lock);

	while(1)
	{
		if(package->number>=package->maxNumber)
		{
			pthread_cond_wait(&package->signal,&package->lock);
		}
		if(package->end)
		{
			pthread_mutex_unlock(&package->lock);
			printf("stop listen client\n");
			return NULL;
		}
		pthread_mutex_unlock(&package->lock);

		cfd = accept(package->sockfd,&temp,&addrLen);
		if(cfd < 0)
		{
			printf("accept error\n");
			continue;
		}
		//临时放一下,记得要删掉
		temp1 = (struct sockaddr_in*)&temp;
		tempIP = (char *)malloc(sizeof(inet_ntoa(temp1->sin_addr))+1);
		if(tempIP == NULL)
		{
			printf("ip malloc fail\n");
		}
		

		bzero(tempIP,sizeof(tempIP));
		strcpy(tempIP,(char *)inet_ntoa(temp1->sin_addr));
		tempPort = ntohs(temp1->sin_port);
		printf("%s:%d connect to the server\n",tempIP,tempPort);
/*  到时候重新写过一次
t1 = time(&t2);
//开始监听，并处理注册
while((t2 - t1)<0.5)
{
	if(0>(flag=recv(cfd,buffer,sizeof(buffer),MSG_DONTWAIT)))
	{
		printf("recv error\n");
		break;
	}
	temp1 = (struct sockaddr_in*)&temp;
	tempIP = inet_ntoa(temp1->sin_addr.s_addr);
	tempPort = htohs(temp1->sin_port);
	 if(flag ==0)
	{
		printf("%s:%d has close the socket\n",tempIP,tempPort);
		break;
	}
	splitString(tempBuffer,buffer);
	if(strcmp(tempBuffer,"register")!=0)
	{
		send(cfd,"please register first\n",23,MSG_DONTWAIT);
		flag = 0;
		break;
	}
	splitString(tempBuffer,buffer+sizeof(tempBuffer)+1);
	
	time(&t2);
	
}
*/
//这里开始完成注册
		pthread_mutex_lock(&package->lock);
		appendNode(&package->clientTree,createNode(tempBuffer,cfd,tempIP,tempPort));
		package->number++;

	}

}

WRAP * initialWrap(int s_sock,int maxNumber)
{
	WRAP *p = (WRAP*)malloc(sizeof(WRAP));
	if(p==NULL)
	{
		return NULL;
	}
	p->clientTree = NULL;
	p->maxNumber = maxNumber;
	p->number = 0;
	p->sockfd = s_sock;
	pthread_mutex_init(&p->lock,0);
	p->end = 0;
	pthread_cond_init(&p->signal,NULL);
	return p;
}

void destroyWrap(WRAP *package)
{
	if(package->clientTree != NULL)
	{
		NODE *p = package->clientTree;
		NODE *p2=NULL;
		while(p!=NULL)
		{
			shutdown(p->id,SHUT_RDWR);
			p2 = p;
			p = p->next;
			//free(p2->username);
			free(p2->ip);
			free(p2);
		}

	}
	pthread_mutex_destroy(&package->lock);
	pthread_cond_destroy(&package->signal);
	free(package);
}

