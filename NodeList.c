#include "NodeList.h"


//返回非零值得代表成功
NODE* deleteNode(NODE **parent,int cfd)
{
	if(cfd<0)
	{
		return NULL;
	}
	if(parent == NULL)
	{
		return NULL;
	}
	NODE *p = *parent;
	while(p->id != cfd)
	{
		p = p->next;
	}
	if(p->prev == NULL)
	{
		if(p->next == NULL)
		{
			free(p->ip);
			free(p);
			*parent = NULL;
			return NULL;
		}
		else
		{
			NODE *p2=p->next;
			p2->prev = NULL;
			free(p->ip);
			free(p);
			*parent = p2;
			return p2;
		}
		
	}
	else if(p->next == NULL)
	{
		p->prev->next = NULL;
		free(p->ip);
		free(p);
		return NULL;
	}
	else
	{
		p->prev->next = p->next;
		p->next->prev = p->prev;
		NODE *p2 = p->prev;
		free(p->ip);
		free(p);
		return p2;
	}
	
	return NULL;
}
//返回非零值得代表成功
int appendNode(NODE **parent,NODE *child)
{
	if(*parent == NULL && child == NULL)
	{
		printf("the Nodelist and child node are null\n");
		return 0;
	}
	if(*parent == NULL)
	{
		printf("the nodelist is NULL\n");
		*parent = child;
		return 1;
	}
	NODE *p = *parent;
	while(p->next != NULL)
	{
		p = p->next;
	}
	p->next = child;
	child->prev = p;
	return 1;
}

NODE * createNode(char *username,int cfd,char *ip,int port)
{
	NODE *p = (NODE *)malloc(sizeof(NODE));
	if(p == NULL)
	{
		printf("create Node fail\n");
		return NULL;
	}
	p->next = NULL;
	p->prev = NULL;
	p->username = username;
	p->id = cfd;
	p->ip = ip;
	p->port =port;
	return p;
}

