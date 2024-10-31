#pragma once
#include"./inetMediator.h"
class TCPClientMediator:public INetMediator {
Q_OBJECT
public:
	TCPClientMediator();
	~TCPClientMediator();
	//初始化网络
	bool openNet();
	//发送数据
	bool sendData(char* data, int len, long to);
	//处理数据
	void dealData(char* data, int len, long from);
	//关闭网络
	bool closeNet();
signals:
    void SIG_dealdata(char* data, int len, long from);
};
