#include"./TCPClientMediator.h"
#include"../inet/TCPClient.h"
TCPClientMediator::TCPClientMediator(){
	mp_inet = new TCPClient;
	mp_inet->mp_Mediator = this;
}
TCPClientMediator::~TCPClientMediator() {
    if(mp_inet){
        delete mp_inet;
        mp_inet=nullptr;
    }
}
//初始化网络
bool TCPClientMediator::openNet() {
	return mp_inet->ConnetInet();
}
//发送数据
bool TCPClientMediator::sendData(char* data, int len, long to) {
	return mp_inet->sendData(data, len,to);
	
}
//处理数据
void TCPClientMediator::dealData(char* data, int len, long from) {
    //交给核心处理类
    Q_EMIT SIG_dealdata(data,len,from);
}
//关闭网络
bool TCPClientMediator::closeNet() {
	mp_inet->unInitNet();
	return true;
}
