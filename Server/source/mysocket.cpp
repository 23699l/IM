#include<mysocket.h>

int SOCKET(int domain,int type,int protocol){
	int sockfd=socket(domain,type,protocol);
	if(sockfd==-1){
		perror("socket create error\n");
		return -1;
	}
	return sockfd;
}
int BIND(int sockfd,const struct sockaddr *addr,socklen_t addrlen){
	int error=bind(sockfd,addr,addrlen);	
	if(error==-1){
		perror("bing error\n");
		return -1;
	}
	return error;
}
int LISTEN(int sockfd,int backlog){
	int error=listen(sockfd,backlog);
	if(error==-1){
		perror("listen error\n");
		return -1;
	}
	return error;
}
int CONNECT(int sockfd,const struct sockaddr *addr,socklen_t addrlen){
	int error=connect(sockfd,addr,addrlen);	
	if(error==-1){
		perror("connect error:\n");
		return -1;
	}
	return error;
}
int ACCEPT(int sockfd,struct sockaddr *addr,socklen_t *addrlen){
	int client_sock=accept(sockfd,addr,addrlen);
	if(client_sock==-1){
		perror("accept error\n");	
	}
	return client_sock;
}
ssize_t SEND(int sockfd,const void *buf,size_t len,int flags){
	ssize_t sendlen;
	sendlen=send(sockfd,buf,len,flags);
	if(sendlen==-1){
		perror("send call failed");
	}
	return sendlen;
}
ssize_t RECV(int sockfd,void *buf,size_t len,int flags){
	ssize_t recvlen;
	recvlen=recv(sockfd,buf,len,flags);
	if(recvlen==-1){
		if(errno==EAGAIN){//非阻塞返回
			printf("recv nonblock, tryagain\n");
		}else{
			perror("recv call failed\n");	
		}
	}
	return recvlen;
}
int NET_INITIALIZER(const char *ip,int ip_len,int port){//初始化服务端网络(TCP)
	struct sockaddr_in server_addr;
	server_addr.sin_port=htons(port);
	server_addr.sin_family=AF_INET;
	if(ip==NULL){
		server_addr.sin_addr.s_addr=htonl(INADDR_ANY);	
	}else{
		inet_pton(AF_INET,ip,&(server_addr.sin_addr.s_addr));	
	}
	int sockfd=SOCKET(AF_INET,SOCK_STREAM,0);
	BIND(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	LISTEN(sockfd,128);
	return sockfd;
}
int EIBST_RESPONSE(int client_sock,const char *client_ip){//基础的回
	char response[8192];
	memset(response,0,sizeof(response));
	sprintf(response,"Hi %s Wellcome to TCP server",client_ip);
	SEND(client_sock,response,strlen(response),MSG_NOSIGNAL);
	return 0;
}
