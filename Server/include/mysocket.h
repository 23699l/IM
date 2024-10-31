#pragma once
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include<ctime>
int SOCKET(int domain,int type,int protocol);
int BIND(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
int LISTEN(int sockfd,int backlog);
int CONNECT(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
int ACCEPT(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
ssize_t SEND(int sockfd,const void *buf,size_t len,int flags);
ssize_t RECV(int sockfd,void *buf,size_t len,int flags);
int NET_INITIALIZER(const char *ip,int ip_len,int port);
int EIBST_RESPONSE(int client_sock,const char *client_ip);
