#pragma once
class INet;
class INetMediator{
public:
	INetMediator():mp_inet(nullptr){}
	virtual ~INetMediator() {}
	//初始化网络
	virtual bool openNet() = 0;
	//发送数据
	virtual bool sendData(char* data, int len, long to) = 0;
	//处理数据
	virtual void dealData(char* data, int len, long from) = 0;
	//关闭网络
	virtual bool closeNet() = 0;
protected:
	INet* mp_inet;
};
