#pragma once
#include"./inet.h"
class UDP :public INet {
public:
	UDP();
	~UDP();
	bool ConnetInet();//初始化网络
	//发送数据
	//UDP传的是ip
	//TCP传的是SCOKET
	bool sendData(char* data, int len, long to);
	void unInitNet();//关闭网络
	void recvData();


};