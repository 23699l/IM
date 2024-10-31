#pragma once
#include"./inetMediator.h"
class TCPServerMediator :public INetMediator {
public:
	TCPServerMediator();
	~TCPServerMediator();
	//初始化网络
	bool openNet();
	//发送数据
	bool sendData(char* data, int len, long to);
	//处理数据
	void dealData(char* data, int len, long from);
	//关闭网络
	bool closeNet();
};