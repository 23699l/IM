#ifndef CKERNAL_H
#define CKERNAL_H
#include <QObject>
#include"./inetMediator/inetMediator.h"
#include"./chatdialog.h"
#include"./inet/packDef.h"
#include"friendlist.h"
#include"useriteam.h"
#include<map>
#include"chat.h"
#include"groupiteam.h"
#include"groupchat.h"
#include"fileiteam.h"
using namespace std;
class CKernal;
typedef void (CKernal::* FUN_PTR)(char*, int, long);
class CKernal : public QObject
{
    Q_OBJECT
public:
    explicit CKernal(QObject *parent = nullptr);
    ~CKernal();
    void Deal_Protocl_Arr();//绑定协议头数组
    void Deal_Register_RE(char* data,int len,long from);
    void Deal_Login_RE(char* data,int len,long from);
    void Deal_UserInfor(char* data,int len,long from);//处理自己信息以及联系人中好友信息
    void Deal_Offline_QS(char* data,int len,long from);//处理下线请求
    void Deal_Chat_QS(char* data,int len,long from);//处理聊天请求
    void Deal_Chat_RE(char* data,int len,long from);//处理聊天回复
    void Deal_AddFriend_QS(char* data,int len,long from);//添加好友请求
    void Deal_AddFriend_RE(char* data,int len,long from);//添加好友回复
    void Deal_OfflineChat_QS(char* data,int len,long from);//处理离线时接收到的聊天内容
    void Deal_CRE_GROUP_RE(char* data,int len,long from);//建群回复
    void Deal_GroupInfor(char* DealBuf, int len, long from);//更新，群iteam
    void Deal_GroupChatQs(char* DealBuf, int len, long from);//处理，接收群信息请求
    void Deal_GroupInviteQS(char* DealBuf, int len, long from);//处理，群聊邀请请求
    void Deal_GroupInviteRE(char* DealBuf, int len, long from);//处理，群聊邀请回复
    void Deal_SendFileInfoQs(char* DealBuf, int len, long from);//处理，文件信息请求
    void Deal_SendFileInfoRe(char* DealBuf, int len, long from);//处理，文件信息回复
    void utf8togb2312(QString utf,char *gb,int len);
    QString gb2312toutf8(char *gb);
private:
    INetMediator* m_pMediator;
    chatDialog *m_dialog;//登入注册
    FUN_PTR Fun_PtrArr[_DEF_PROTOCOL_COUNT];
    FriendList* m_pfriendList;//好友列表
    map<int,useriteam*> m_idToUseriteam;
    map<int,Chat*> m_idToChat;
    map<int,GroupChat*> m_GroupIdToGroupChat;
    map<int,groupiteam*> m_GroupIdToGroupIteam;
    map<string,FILEINFO*> m_FileIdToFileInfo;
    map<string,fileIteam*> m_FileIdToFileIteam;
    int m_id;//自己的id
    QString m_name;//自己的昵称
signals:
private slots:
     void SLOT_SendContent(QString content,int friend_id);//发送聊天内容
     void SLOT_dealdata(char* data, int len, long from);
     void SLOT_RegisterMessage(QString tel,QString password,QString nickname);
     void SLOT_LoginMessage(QString tel,QString password);
     void SLOT_closeApp();
     void SLOT_offline();//下线处理
     void SLOT_ShowChat(int id);//展示聊天窗口的槽函数
     void SLOT_addFriend();//添加好友的槽函数
     void SLOT_CreateGroup();//创建群聊
     void SLOT_showGroupChat(int id);
     void SLOT_GroupSendContent(QString ,int);//群chat发送消息
     void SLOT_GroupInviteMember(int gId,QString group_name);//邀请好友槽函数
     void SLOT_SendFileInfoQs(string ,int,int,int,int,int,FILE*,string);//文件信息请求
};
#endif // CKERNAL_H
