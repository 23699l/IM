#pragma once
#include<iostream>
#include<list>
//#include"./packDef.h"
#include<packDef.h>
#include<map>
#include<mysocket.h>
#include<set>
using namespace std;
class INetMediator;
class INet {
	public:
		INet():mp_Mediator(nullptr){}
		virtual ~INet() {}
		virtual bool ConnetInet()=0;//初始化网络
		//发送数据
		//UDP传的是ip
		//TCP传的是SCOKET
		virtual bool sendData(char *data,int len,long to) = 0;
		virtual void unInitNet() = 0;//关闭网络
		virtual void recvData(int *p_sockfd)=0;
		//virtual void recvData() = 0;
	public:
		INetMediator* mp_Mediator;
};
