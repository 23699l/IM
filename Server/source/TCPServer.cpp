#include<TCPServer.h>
//#include"../inetMediator/TCPServerMediator.h"
#include<TCPServerMediator.h>
int epollfd;
TCPServer::TCPServer():m_quit(false),m_sock(-1){}
TCPServer::~TCPServer() {
	unInitNet();
}
void* TCPServer::recvThread(void* lpVoid){
	TCPServer* p_This = (TCPServer*)(((busines_arg* )lpVoid)->arg_this);
	int *sockfd=(int*)(((busines_arg* )lpVoid)->arg_sock);
	p_This->recvData(sockfd);
	return nullptr;
}
void* TCPServer::acceptThread(void* lpVoid){
	TCPServer* p_This=(TCPServer*)(((busines_arg* )lpVoid)->arg_this);
	int sockfd=*(int*)(((busines_arg* )lpVoid)->arg_sock);
	sockaddr_in sockaddr_client;
	int client_len = sizeof(sockaddr_in);
	char client_ip[16]="";
	pthread_t threadId=0;
	int err;
	int sock_talk=ACCEPT(sockfd,(sockaddr*)&sockaddr_client,(socklen_t*)&client_len);
	if(-1!=sock_talk){//连接成功
		//打印客户信息
		inet_ntop(AF_INET,&sockaddr_client.sin_addr.s_addr,client_ip,16);
		cout << "client ip:" <<client_ip<< endl;
		auto pr=(p_This->msock_set).insert(sock_talk);
		if(!(pr.second)){
			cout<<"客户的socket,ip:"<< client_ip<<"保存erro"<<endl;	
		}
		//把新socket加入epoll监听树
		struct epoll_event node;
		node.data.fd=sock_talk;
		node.events=EPOLLIN|EPOLLONESHOT;
		err=epoll_ctl(epollfd,EPOLL_CTL_ADD,sock_talk,&node);
		if(err==-1){
			perror("TCPServer::acceptThread: epoll_ctl_add socket_talk error");
		}
	}
	//把接受连接的socket挂回树上
	struct epoll_event ev;
	ev.data.fd=sockfd;
	ev.events=EPOLLIN|EPOLLONESHOT;
	err=epoll_ctl(epollfd,EPOLL_CTL_MOD,sockfd,&ev);
	if(err==-1){
		perror("TCPServer::acceptThread:把 sockfd添加回红黑树 error");
	}
	return nullptr;
}
bool TCPServer::ConnetInet() {
	//创建套接字
	m_sock=SOCKET(AF_INET,SOCK_STREAM,0);
	if(m_sock==-1){
		return false;
	}
	sockaddr_in sockaddr_server;
	inet_pton(AF_INET,TCP_SERVER_IP,&sockaddr_server.sin_addr.s_addr);
	sockaddr_server.sin_family = AF_INET;
	sockaddr_server.sin_port = htons(TCP_PORT);
	//绑定
	int err;
	err=BIND(m_sock,(sockaddr*)&sockaddr_server,sizeof(sockaddr_server));
	if(err==-1){
		return false;	
	}
	//监听
	err=LISTEN(m_sock,128);
	if(err==-1){
		return false;	
	}
	//epoll+线程池
	pool_t *pt=create_pool(8,4,10000);
	epoll_Init(m_sock);
	epoll_Listen(m_sock,pt);
	return true;
}
int TCPServer::Is_Thread_Alive(pthread_t tid){
	int error=pthread_kill(tid,0);
	if(error ==ESRCH){
		return 0;	
	}else{//该线程还存活
		return 1;	
	}
}
int TCPServer::epoll_Init(int sockfd){
	//创建监听树
	epollfd=epoll_create(LISTEN_MAX);
	if(epollfd<0){
		printf("epoll_create error\n");
		return -1;
	}
	struct epoll_event node;
	node.data.fd=sockfd;
	node.events=EPOLLIN|EPOLLONESHOT;
	if(epoll_ctl(epollfd,EPOLL_CTL_ADD,sockfd,&node)==-1){
		perror("epoll_ctr EPOLL_CTL_ADD error\n");
		return -1;
	}
	return 0;
}
//创建线程池
pool_t* TCPServer::create_pool(int max,int min,int queue_max){
	pool_t* pt=(pool_t*)malloc(sizeof(pool_t)); 
	pt->THREAD_SHUTDOWN=0;//开启线程池
	pt->THREAD_MAX=max;
	pt->THREAD_MIN=min;
	pt->THREAD_ALIVE=0;
	pt->THREAD_BUSY=0;
	pt->THREAD_EXITCODE=0;
	//初始化队列
	pt->queue=(busines_t*)malloc(sizeof(busines_t)*queue_max);
	pt->queue_max=queue_max;
	pt->queue_front=0;
	pt->queue_rear=0;
	pt->queue_cur=0;
	//初始化条件变量
	if((0!=pthread_cond_init(&pt->full,NULL))||(0!=pthread_cond_init(&pt->empty,NULL))){
		printf("条件变量初始化失败\n");
	}
	//初始化锁
	if(0!=(pthread_mutex_init(&pt->lock,NULL))){
		printf("锁初始化错误\n");
	}
	int error;
	//创建消费者线程id数组并创建线程和存储id
	pt->ctids=(pthread_t*)malloc(sizeof(pthread_t)*(pt->THREAD_MAX));
	memset(pt->ctids,0,sizeof(pthread_t)*(pt->THREAD_MAX));
	for(int i=0;i<pt->THREAD_MIN;i++){
		if((error=pthread_create((pt->ctids)+i,NULL,&TCPServer::customer_job,(void *)pt))>0){
			printf("create thread error : %s\n",strerror(error));
			break;
		}else{
			pt->THREAD_ALIVE++;
			printf("create_pool:creatr thread success\n");
		}
	}
	//创建管理者并存储id
	if((error=pthread_create(&(pt->mid),NULL,&TCPServer::manager_job,(void *)pt))>0){
		printf("create thread error : %s\n",strerror(error));
	}
	return pt;
	//______________________	
}
int TCPServer::epoll_Listen(int sockfd,pool_t *pt){
	int ready=0;
	struct epoll_event ready_arr[LISTEN_MAX];
	busines_t bs;
	//循环监听
	while(!pt->THREAD_SHUTDOWN){
		if((ready=epoll_wait(epollfd,ready_arr,LISTEN_MAX,-1))==-1){
			perror("epoll_wait error:");	
		}
		for(int i=0;i<ready;i++){
			//把任务添加到队列
			if(ready_arr[i].data.fd==m_sock){
				bs.business=&acceptThread;//任务函数
				bs.arg.arg_sock=(void *)&ready_arr[i].data.fd;//参数
				bs.arg.arg_this=(void *)this;//参数
			}else{
				bs.business=&recvThread;//任务函数
				bs.arg.arg_sock=(void *)&ready_arr[i].data.fd;//参数
				bs.arg.arg_this=(void *)this;//参数
			}
			productor_add(pt,bs);
		}
	}
	printf("SHUTDOWN close main exit:0x%x\n",(unsigned int)pthread_self());
	close(epollfd);
	pool_destroy(pt);
	close(sockfd);
	pthread_exit(NULL);
	return 0;	
}
//生产者添加任务
int TCPServer::productor_add(pool_t *pt,busines_t bs){
	//访问队列
	if(!pt->THREAD_SHUTDOWN){
		//上锁
		pthread_mutex_lock(&(pt->lock));
		//判断队列是否已经满
		if(pt->queue_cur==pt->queue_max){
			//满者挂起
			pthread_cond_wait(&(pt->full),&(pt->lock));
			if(pt->THREAD_SHUTDOWN){//线程退出
				return 0;
			}
		}
		//生产者被唤醒者继续添加
		(pt->queue[pt->queue_rear]).business=bs.business;
		(pt->queue[pt->queue_rear]).arg=bs.arg;
		(pt->queue_cur)++;
		pt->queue_rear=((pt->queue_rear)+1)%(pt->queue_max);
		//唤醒一个线程
		pthread_cond_signal(&(pt->empty));
		//解锁添加完成
		pthread_mutex_unlock( &(pt->lock) );
		printf("添加任务成功\n");
		return 1;
	}
	printf("producer_addr: THREAD_SHUTDOWN close\n");
	return 0;
}
void TCPServer::pool_destroy(pool_t *pt){
	//关闭线程池
	pt->THREAD_SHUTDOWN=1;
	//销毁条件变量
	pthread_cond_destroy(&(pt->full));
	pthread_cond_destroy(&(pt->empty));
	//回收锁
	pthread_mutex_destroy(&(pt->lock));
	//回收队列
	free(pt->queue);
	free(pt);
}
//管理者工作
void* TCPServer::manager_job(void* arg){
	pool_t* pt=(pool_t*)arg;
	int busy_thread;
	int alive_thread;
	int cur;//队中的任务数
	int erro;
	int add;//创建线程的数量
	int flage;
	while(!pt->THREAD_SHUTDOWN){
		pthread_mutex_lock(&(pt->lock));
		busy_thread=pt->THREAD_BUSY;
		alive_thread=pt->THREAD_ALIVE;
		cur=pt->queue_cur;
		pthread_mutex_unlock(&(pt->lock));
		printf("ALIVE_THREAD:%d BUSY_THREAD:%d Idel:%d queue_cur:%d b/a:%f\n",alive_thread,busy_thread,alive_thread-busy_thread,cur,((float)busy_thread/alive_thread)*100);

		//判断是否要扩容
		if(  ( (cur>=alive_thread-busy_thread)|| (((float)busy_thread/alive_thread)*100>=70) )&&(alive_thread+pt->THREAD_MIN<=pt->THREAD_MAX)

		  ){
			for(add=0,flage=0;(flage<(pt->THREAD_MAX))&&(add<(pt->THREAD_MIN));flage++){
				if(pt->ctids[flage]==0||!TCPServer::Is_Thread_Alive(pt->ctids[flage])){
					if((erro=pthread_create(&(pt->ctids[flage]),NULL,&customer_job,(void*)pt))>0){
						printf("manager_job:pthread_create create error:%s\n",strerror(erro));
					}
					add++;
					pthread_mutex_lock(&(pt->lock));
					(pt->THREAD_ALIVE)++;
					pthread_mutex_unlock(&(pt->lock));
				}	
			}
		}

		//判断是否要缩减
		if((busy_thread*2<=alive_thread-busy_thread)&&(pt->THREAD_MIN<=(alive_thread-pt->THREAD_MIN))){
			pthread_mutex_lock(&pt->lock);
			pt->THREAD_EXITCODE=pt->THREAD_MIN;
			pthread_mutex_unlock(&pt->lock);
			for(int i=0;i<pt->THREAD_MIN;i++){
				pthread_cond_signal(&pt->empty);
			}
		}
		sleep(2);
	}
	printf("manager exit SHUTDOWN close\n");
	pthread_exit(NULL);
}
//消费者工作
void* TCPServer::customer_job(void *arg){
	pool_t* pt=(pool_t*)arg;
	busines_t bs;
	//判断线程池是否要退出
	while(!pt->THREAD_SHUTDOWN){
		//上锁
		pthread_mutex_lock(&(pt->lock));
		//判断队列是否为空
		while((pt->queue_cur)==0){
			pthread_cond_wait(&(pt->empty),&(pt->lock));
			if(pt->THREAD_SHUTDOWN){//判断线程池是否退出
				pthread_mutex_unlock(&(pt->lock));	
				pthread_exit(NULL);
			}
			if(pt->THREAD_EXITCODE>0){
				printf("custormer exit 0x%x\n",(unsigned int)pthread_self());
				(pt->THREAD_EXITCODE)--;
				(pt->THREAD_ALIVE)--;
				pthread_mutex_unlock(&(pt->lock));
				pthread_exit(NULL);
			}
		}
		//繁忙线程+1
		(pt->THREAD_BUSY)++;
		bs.business=(pt->queue[pt->queue_front]).business;
		bs.arg.arg_sock=(pt->queue[pt->queue_front]).arg.arg_sock;
		bs.arg.arg_this=(pt->queue[pt->queue_front]).arg.arg_this;
		(pt->queue_cur)--;
		(pt->queue_front)=(pt->queue_front+1)%(pt->queue_max);
		//唤醒生产者
		pthread_cond_signal(&(pt->full));
		//解锁
		pthread_mutex_unlock(&(pt->lock));
		//执行任务
		printf("thread 0x%x runing\n",(unsigned int)pthread_self());

		(bs.business)((void*)&bs.arg);
		//上锁
		pthread_mutex_lock(&pt->lock);
		//繁忙线程--
		(pt->THREAD_BUSY)--;
		//解锁
		pthread_mutex_unlock(&pt->lock);
	}	
	printf("customer 0x%x exit PTHRAED_SHUTDOWN close\n",(unsigned int)pthread_self());
	pthread_exit(NULL);
}
bool TCPServer::sendData(char* data, int len, long to) {

	if (len <= 0 && data == nullptr) {
		return false;
	}

	//先发包大小
	int erro = SEND(to, (const char*)&len, sizeof(int),MSG_NOSIGNAL);
	if (erro == -1) {
		return false;
	}

	//再发包内容
	erro = SEND(to, data, len, MSG_NOSIGNAL);
	if (erro == -1) {
		return false;
	}
	return true;
}
void TCPServer::unInitNet(){
	//退出线程
	m_quit = true;
	//关闭套接字
	if (m_sock && m_sock !=-1) {
		close(m_sock);
		m_sock=-1;
	}
	for (auto ite = msock_set.begin(); ite != msock_set.end();) {
		int sock = *ite; 
		if (sock && sock != -1) {
			close(sock);
		}
		sock = -1;
		ite = msock_set.erase(ite);
	}
}
void TCPServer::recvData(int* p_sockfd){
	int sock_talk=*p_sockfd;
	if (!sock_talk&&sock_talk == -1) {
		cout << "TCPServer::recvData: sock_talk error" << endl;
		return;
	}
	int recv_len = 0;//接受了多少字节
	char* recv_buf = nullptr;//注意长度可能会溢出
	int packet_len = 0;//接收到的包大小
	int remain_len = 0;//剩余未接收的包大小
	//先接包大小
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
				cout << "TCPClient::recvData: 接收数据失败"<< endl;
				break;
			}
		}
		//接收到的数据交给中介者
		mp_Mediator->dealData(recv_buf, packet_len, sock_talk);
		//回收recv_buf
		if (recv_buf) {
			delete []recv_buf;
			recv_buf = nullptr;
		}
		//把sock_talk挂回树上
		struct epoll_event ev;
		ev.data.fd=sock_talk;
		ev.events=EPOLLIN|EPOLLONESHOT;
		int err=epoll_ctl(epollfd,EPOLL_CTL_MOD,sock_talk,&ev);
		if(err==-1){
			perror("TCPServer::recvData:把sock_talke 添加回红黑树 error:\n");
		}
	}else{
		cout << "TCPClient::recvData: 接收数据失败"<<endl;
	}
}
