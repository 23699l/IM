#pragma once
#include"./inet.h"
class TCPClient :public INet {
public:
	TCPClient();
	~TCPClient();
	bool ConnetInet();//初始化网络
	//发送数据
	//UDP传的是ip
	//TCP传的是SCOKET
	bool sendData(char* data, int len, long to);
	void unInitNet();//关闭网络
	void recvData();
protected:
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	bool m_quit;
	HANDLE m_handle;

};