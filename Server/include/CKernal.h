#pragma once
#include<iostream>
#include<inetMediator.h>
#include<CMySql.h>
#include<packDef.h>
#include<list>
#include<string>
#include<map>
using namespace std;
class CKernal;
typedef void (CKernal::* FUN_PTR)(char*, int, long);
using namespace std;
class CKernal {
public:
	CKernal();
	~CKernal();
	bool startNet();
	bool closeNet();
	void dealData(char* data, int len, long from);
	static CKernal* m_kernal;
	void Deal_Chat_QS(char* data, int len, long from);//处理聊天请求
	void Deal_Login_QS(char* data, int len, long from);//处理登入请求
	void Deal_Register_QS(char* data, int len, long from);//处理注册请求
	void Deal_Offline_QS(char* data, int len, long from);//处理下线请求
	void Deal_AddFriend_QS(char* data, int len, long from);//添加好友请求
	void Deal_AddFriend_RE(char* data, int len, long from);//添加好友回复
	void Deal_CreGroup_QS(char* data, int len, long from);//处理创建群聊请求
	void Deal_GroupInvite_QS(char* data, int len, long from);//群聊邀请好友请求
	void Deal_GroupInvite_RE(char* data, int len, long from);//群聊邀请好友回复
	void Deal_GroupChatQs(char* data, int len, long from);//群聊消息请求
	void Deal_Protocl_Arr();//绑定协议头数组
	void Deal_SendFileInfoQs(char* data, int len, long from);//发送文件信息请求
	void Deal_SendFileInfoRe(char* data, int len, long from);//发送文件信息回复
	void Deal_FileContent(char* data, int len, long from);//处理文件内容
	void getUserInfor(int userId);//得到用户信息以及好友信息
	void getInforByUserId(int userId, STU_TCP_USERINFORMATION *user_infor);
	void getOfflineChat(int user_id);//转发该用户，离线时收到的消息
	void getGroupInfor(int user_id);//转发群iteam信息
	CMySql m_MySql;
private:
	INetMediator* p_mMediator;
	//协议头数组
	FUN_PTR Fun_PtrArr[_DEF_PROTOCOL_COUNT];
	map<int, int> m_idToSock;//保存在线用户的id—socket

};
