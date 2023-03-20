#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
	struct Node *next;
	struct Node *prev;
	char *username;
	int id;				//id就是socket
	char *ip;
	int port;
}NODE;

//返回非零值得代表成功
int appendNode(NODE **parent,NODE *child);
//成功返回非零值
NODE* deleteNode(NODE **parent,int cfd);
//
NODE * createNode(char *username,int cfd,char *ip,int port);
