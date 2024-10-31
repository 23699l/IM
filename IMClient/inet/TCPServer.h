#pragma once
#include"./inet.h"
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
	void recvData();
	static unsigned __stdcall acceptThread(void* lpVoid);
	static unsigned __stdcall TCPServer::recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	list<HANDLE> mHandle_List;
	map<unsigned int,SOCKET>msock_map;
	bool m_quit;
	
};