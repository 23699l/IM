#include<TCPServerMediator.h>
#include<TCPServer.h>
#include<CKernal.h>
TCPServerMediator::TCPServerMediator(){
	mp_inet = new TCPServer;
	mp_inet->mp_Mediator = this;
}
TCPServerMediator::~TCPServerMediator() {}
//初始化网络
bool TCPServerMediator::openNet() {
	return mp_inet->ConnetInet();
}
//发送数据
bool TCPServerMediator::sendData(char* data, int len, long to){
	return mp_inet->sendData(data,len,to);
}
//处理数据
void TCPServerMediator::dealData(char* data, int len, long from){
	//cout << __FUNCTION__ << data << endl;
	CKernal::m_kernal->dealData(data, len, from);
	
}
//关闭网络
bool TCPServerMediator::closeNet() {
	mp_inet->unInitNet();
	return true;
}
