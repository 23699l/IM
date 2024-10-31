#pragma once
//#include"./inet.h"
#include<inet.h>
#include<pool_server.h>
class TCPServer :public INet {
	public:
		TCPServer();
		~TCPServer();
		bool ConnetInet();//初始化网络
		//发送数据
		//UDP传的是ip
		//TCP传的是SCOKET
		bool sendData(char* data, int len, long to);
		void unInitNet();//关闭网络
		void recvData(int *p_sockfd);
		pool_t* create_pool(int max,int min,int queue_max);//创建线程池
		int epoll_Init(int sockfd);//初始化epoll
		int epoll_Listen(int sockfd,pool_t *pt);//epoll监听函数
		static int Is_Thread_Alive(pthread_t);//检测线程是否还存活,线程存活返回true
		void pool_destroy(pool_t*);//销毁线程池
		int productor_add(pool_t *,busines_t);//生产者添加任务
		static void* acceptThread(void* lpVoid);//接受连接
		static void* recvThread(void* lpVoid);//接收消息
		static void* customer_job(void *);//消费者线程工作
		static void* manager_job(void *);//管理者线程工作
	private:
		int m_sock;
		set<int> msock_set;//客户端sock
		bool m_quit;
};
