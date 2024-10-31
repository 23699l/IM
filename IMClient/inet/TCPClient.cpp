#include"./TCPClient.h"
#include<QDebug>
#include"../inetMediator/TCPClientMediator.h"
TCPClient::TCPClient() :m_sock(INVALID_SOCKET), m_quit(false), m_handle(NULL) {}
TCPClient::~TCPClient() {
	unInitNet();
}
//接受数据线程函数
unsigned __stdcall TCPClient::recvThread(void* lpVoid) {
	TCPClient* p_TcpClient=(TCPClient*)lpVoid;
	p_TcpClient->recvData();
	return 1;
}
bool TCPClient::ConnetInet() {
	//加载库
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	int error=WSAStartup(version,&data);
	if (error != 0) {
		cout << "WSAStartup error\n"<<WSAGetLastError() << endl;
		return false;
	}
	//判断版本号
	if (HIBYTE(data.wVersion) != 2 || LOBYTE(data.wVersion) != 2) {
		cout << "WSAStartup version error" << endl;
		return false;
	}
	//创建套接字
	m_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
		cout << "sock 无效" << endl;
		return false;
	}
	//阻塞连接
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(TCP_PORT);
	server_addr.sin_addr.S_un.S_addr = inet_addr(TCP_SERVER_IP);
	error=connect(m_sock, (sockaddr*)&server_addr, sizeof(server_addr));
	if (error == SOCKET_ERROR) {
        qDebug()<<"connect error";
		return false;
	}
	else {
        qDebug() << "与服务器连接成功" ;
	}
	//连接成功开启一个线程，来接收消息
	m_handle = (HANDLE)_beginthreadex(0, 0, recvThread, (void*)this, 0, NULL);
	return true;
}
bool TCPClient::sendData(char* data, int len, long to) {
	if (len <= 0 && data == nullptr) {
		return false;
	}

	//先发包大小
	int erro=send(m_sock, (const char *) &len, sizeof(int), 0);
	if (erro <= 0) {
		cout << "send error" <<WSAGetLastError()<< endl;
		return false;
	}

	//再发包内容
	erro =send(m_sock,data,len,0);
	if (erro <= 0) {
		cout << "send error" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
void TCPClient::unInitNet() {


	//回收线程
	m_quit = true;
	if (m_handle) {
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 500)){
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}
	//关闭套接字
	if (m_sock && m_sock != INVALID_SOCKET) {
		closesocket(m_sock);
	}
	//卸载库
	WSACleanup();

}//关闭网络
void TCPClient::recvData() {
	int recv_len = 0;//接受了多少字节
	char *recv_buf=nullptr;//注意长度可能会溢出
	int packet_len = 0;//接收到的包大小
	int remain_len = 0;//剩余未接收的包大小
	while (!m_quit) {
		//先接包大
		recv_len = recv(m_sock, (char*)&packet_len, sizeof(packet_len), 0);
		//判断接收包大小是否成功
		if (recv_len > 0) {
			recv_buf = new char[packet_len];
			remain_len = packet_len;
			while (remain_len > 0) {
				//再接包内容
				recv_len = recv(m_sock, recv_buf + (packet_len - remain_len), remain_len, 0);
				if (recv_len > 0) {
					remain_len -= recv_len;
				}else{
                    qDebug() << "TCPClient::recvData 错误" << WSAGetLastError();
					break;
				}
			}
			//接收到的数据交给中介者
			//cout << "server say:" << recv_buf << endl;
			mp_Mediator->dealData(recv_buf, packet_len, m_sock);
		}else {
            qDebug() << "TCPClient::recvData 错误" <<WSAGetLastError();
			break;
		}
	}
}
