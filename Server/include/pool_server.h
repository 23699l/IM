#pragma once
#include<stdio.h> 
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<signal.h>
#include<errno.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
typedef struct{
	void* arg_sock;
	void* arg_this;
}busines_arg;
typedef struct{
	void* (*business)(void*); 	
	busines_arg arg;
}busines_t;
extern int epollfd;
#define LISTEN_MAX 100000
typedef struct{
	int THREAD_SHUTDOWN;//1为退出线程池
	int THREAD_MAX;//线程的最大数量
 	int THREAD_MIN;//线程的最小数量
	int THREAD_ALIVE;//线程的当前数量
	int THREAD_BUSY;//繁忙的线程数量
	int THREAD_EXITCODE;//退出码,1为需要右线程退出
	busines_t *queue;
	int queue_max;
	int queue_cur;//当前数量
	int queue_front;
	int queue_rear;
	pthread_cond_t full;//给生产者用
	pthread_cond_t empty;//给消费者用
	pthread_mutex_t lock;
	pthread_t *ctids;//线程池中的线程id
	pthread_t mid;//管理者线程id
}pool_t;
