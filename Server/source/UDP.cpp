#include"UDP.h"
UDP::UDP() {}
UDP::~UDP() {}
bool UDP::ConnetInet() {
	return true;
}//初始化网络
bool UDP::sendData(char* data, int len, long to) {
	return true;
}
void UDP::unInitNet() {}//关闭网络
void UDP::recvData() {}