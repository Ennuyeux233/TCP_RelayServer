#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "NodeList.h"
#include <time.h>
#include <errno.h>
#include <sys/select.h>

typedef struct wrap
{
	NODE * clientTree;
	int sockfd;				//服务器的监听端口
	int number; 			//当前客户端数量
	int maxNumber;			//最大客户端数量
	pthread_mutex_t lock;	//访问客户端的锁
	pthread_cond_t signal;
	int end;				//判断是否结束应用
}WRAP;

//初始化wrap结构体
WRAP * initialWrap(int s_sock,int maxNumber);

//成功则返回socket值
int initialServerNetwork(int port,int listenNumber);

//监听客户端
void *listenClient(void *package);

//将信息发送给其他客户端  
void transferMessage(WRAP *package,int cfd,char *message);

//销毁wrap结构体
void destroyWrap(WRAP * package);
