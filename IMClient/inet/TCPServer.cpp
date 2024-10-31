#include"TCPServer.h"
#include"../inetMediator/TCPServerMediator.h"
TCPServer::TCPServer():m_quit(false),m_sock(INVALID_SOCKET){}
TCPServer::~TCPServer() {
	unInitNet();
}
unsigned __stdcall TCPServer::recvThread(void* lpVoid){
	TCPServer* p_This = (TCPServer*)lpVoid;
	p_This->recvData();
	return 1;
}
unsigned __stdcall TCPServer::acceptThread(void* lpVoid){
	TCPServer* p_This=(TCPServer*)lpVoid;
	sockaddr_in sockaddr_client;
	int client_len = sizeof(sockaddr_in);
	unsigned int threadId=0;
	while (!(p_This->m_quit)) {
		SOCKET sock_talk = accept(p_This->m_sock, (sockaddr*)&sockaddr_client, &client_len);
		//打印客户信息
		if (INVALID_SOCKET == sock_talk){
			cout << "accept error:" <<WSAGetLastError() <<endl;
		}else {
			cout << "client ip:" << inet_ntoa(sockaddr_client.sin_addr) << endl;
			//创建接收数据的线程
			HANDLE handl=(HANDLE)_beginthreadex(0, 0, &recvThread,(void *)p_This,0, &threadId);
			if (handl != NULL) {
				//绑定线程ID与客户端sock
				p_This->msock_map[threadId] = sock_talk;
				p_This->mHandle_List.push_back(handl);
			}else{
				cout << "TCPServer::acceptThread: recvThread _beginthreadex error" << endl;
				closesocket(sock_talk);
			}
		}
	}
	return 1;
}
bool TCPServer::ConnetInet() {
	//加载库
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	int error = WSAStartup(version, &data);
	if (error != 0) {
		cout << "WSAStartup error\n" << WSAGetLastError() << endl;
		return false;
	}
	//判断版本号
	if (HIBYTE(data.wVersion) != 2 || LOBYTE(data.wVersion) != 2) {
		cout << "WSAStartup version error" << endl;
		return false;
	}
	//创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
		cout << "sock 无效" << endl;
		return false;
	}
	sockaddr_in sockaddr_server;
	sockaddr_server.sin_addr.S_un.S_addr = inet_addr(TCP_SERVER_IP);
	sockaddr_server.sin_family = AF_INET;
	sockaddr_server.sin_port = htons(TCP_PORT);
	//绑定
	error=bind(m_sock,(sockaddr*) & sockaddr_server,sizeof(sockaddr_server));
	if (error == SOCKET_ERROR) {
		cout << "bind error" << endl;
		return false;
	}
	//监听
	error=listen(m_sock,120);
	if (error == SOCKET_ERROR) {
		cout << "listen error" << endl;
		return false;
	}
	//创建线程，循环接受连接
	HANDLE handl = (HANDLE)_beginthreadex(0, 0, &acceptThread,(void *)this, 0,NULL);
	if (handl) {
		cout << "循环接收连接线程创建成功" << endl;
		mHandle_List.push_back(handl);
	}
	return true;
}
bool TCPServer::sendData(char* data, int len, long to) {

	if (len <= 0 && data == nullptr) {
		return false;
	}

	//先发包大小
	int erro = send(to, (const char*)&len, sizeof(int), 0);
	if (erro <= 0) {
		cout << "send error" << WSAGetLastError() << endl;
		return false;
	}

	//再发包内容
	erro = send(to, data, len, 0);
	if (erro <= 0) {
		cout << "send error" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
void TCPServer::unInitNet(){
	//退出线程
	m_quit = true;
	for (auto ite = mHandle_List.begin(); ite!= mHandle_List.end();) {
		if ((*ite) != nullptr) {
			if (WAIT_TIMEOUT == WaitForSingleObject((*ite), 200)) {
				TerminateThread((*ite), -1);
			}
			CloseHandle((*ite));
			(*ite) = nullptr;
		}
		ite=mHandle_List.erase(ite);
	}
	//关闭套接字
	if (m_sock && m_sock != INVALID_SOCKET) {
		closesocket(m_sock);
	}
	for (auto ite = msock_map.begin(); ite != msock_map.end();) {
		SOCKET sock = ite->second; 
		if (sock && sock != INVALID_SOCKET) {
			closesocket(sock);
		}
		sock = INVALID_SOCKET;
		ite = msock_map.erase(ite);
	}
	//卸载库
	WSACleanup();
}
void TCPServer::recvData() {
	SOCKET sock_talk=this->msock_map[GetCurrentThreadId()];
	if (!sock_talk&&sock_talk == INVALID_SOCKET) {
		cout << "TCPServer::recvData: sock_talk error" << endl;
		return;
	}
	int recv_len = 0;//接受了多少字节
	char* recv_buf = nullptr;//注意长度可能会溢出
	int packet_len = 0;//接收到的包大小
	int remain_len = 0;//剩余未接收的包大小
	while (!m_quit) {
		//先接包大
		recv_len = recv(sock_talk, (char*)&packet_len, sizeof(packet_len), 0);
		//判断接收包大小是否成功
		if (recv_len > 0) {
			recv_buf = new char[packet_len];
			remain_len = packet_len;
			while (remain_len > 0) {
				//再接包内容
				recv_len = recv(sock_talk, recv_buf + (packet_len - remain_len), remain_len, 0);
				if (recv_len > 0) {
					remain_len -= recv_len;
				}
				else {
					cout << "TCPClient::recvData: 接收数据失败" << WSAGetLastError() << endl;
					break;
				}
			}
			//接收到的数据交给中介者
			//cout << "client say:" << recv_buf << endl;
			//sendData("Holle client",sizeof("Holle client"), sock_talk);
			mp_Mediator->dealData(recv_buf, packet_len, sock_talk);
		}
		else {
			cout << "TCPClient::recvData: 接收数据失败" << WSAGetLastError() << endl;
			break;
		}
	}
	//----------------------
}