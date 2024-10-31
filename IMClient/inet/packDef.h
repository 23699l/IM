#pragma once
#include<string.h>
#include<stdio.h>
#define FILE_MAX_PATH 512
#define FILE_CONTENT_SIZE (8*1024)
#define TCP_SERVER_IP           ("192.168.203.130")
#define TCP_PORT                (13456)
#define _DEF_MAX_LEN            (200)
#define _DEF_CONTENT_LEN        (4096)

#define _DEF_PROTOCOL_COUNT     (20)//协议头数组的长度
//协议头
#define _DEF_PROTOCOL_BASE      (1000)
//注册请求
#define _DEF_TCP_REGISTER_QS    (_DEF_PROTOCOL_BASE+1)
//注册回复
#define _DEF_TCP_REGISTER_RE    (_DEF_PROTOCOL_BASE+2)
//登入请求
#define _DEF_TCP_LOGIN_QS       (_DEF_PROTOCOL_BASE+3)
//登入回复
#define _DEF_TCP_LOGIN_RE       (_DEF_PROTOCOL_BASE+4)
//聊天请求
#define _DEF_TCP_CHAT_QS        (_DEF_PROTOCOL_BASE+5)
//聊天回复
#define _DEF_TCP_CHAT_RE        (_DEF_PROTOCOL_BASE+6)
//添加好友请求
#define _DEF_TCP_ADD_FRIEND_QS  (_DEF_PROTOCOL_BASE+7)
//添加好友回复
#define _DEF_TCP_ADD_FRIEND_RE  (_DEF_PROTOCOL_BASE+8)
//下线请求
#define _DEF_TCP_OFFLINE_QS     (_DEF_PROTOCOL_BASE+9)
//用户信息协议请求协议
#define _DEF_TCP_USERINFORMATION     (_DEF_PROTOCOL_BASE+10)
//离线聊天请求
#define _DEF_TCP_OFFLINECHAT    (_DEF_PROTOCOL_BASE+11)
//群聊创建请求
#define _DEF_TCP_CRE_GROUP_QS   (_DEF_PROTOCOL_BASE+12)
//群聊创建回复
#define _DEF_TCP_CRE_GROUP_RE   (_DEF_PROTOCOL_BASE+13)
//群聊iteam，这里与用户iteam分开
#define _DEF_TCP_GROUPINFORMATION   (_DEF_PROTOCOL_BASE+14)
//群聊请求
#define _DEF_TCP_GROUPCHATQS        (_DEF_PROTOCOL_BASE+15)
//群聊邀请请求
#define _DEF_TCP_GROUPINVITEQS        (_DEF_PROTOCOL_BASE+16)
//群聊邀请回复
#define _DEF_TCP_GROUPINVITERE        (_DEF_PROTOCOL_BASE+17)
//文件信息请求
#define _DEF_TCP_FILEINFO_QS          (_DEF_PROTOCOL_BASE+18)
//文件内容
#define _DEF_TCP_FILEBLOCK_QS          (_DEF_PROTOCOL_BASE+19)
//文件请求信息回复
#define _DEF_TCP_FILEINFO_RE          (_DEF_PROTOCOL_BASE+20)
//返回结果
//注册结果
#define _register_success       (0)
#define _register_tel_repeat    (1)
#define _register_name_repeat   (2)

//登入结果
#define _login_success          (0)
#define _login_no_this_user     (1)
#define _login_password_error   (2)
#define _login_user_have_login  (3)//用户已经登入
//添加好友结果
#define _addfriend_success      (0)
#define _addfriend_not_exists   (1)
#define _addfriend_offline      (2)
#define _addfriend_refuse       (3)
//聊天结果
#define _chat_send_success      (0)
#define _chat_send_error        (1)
//用户状态
#define _user_state_online      (0)
#define _user_state_offoline    (1)
//离线保存，聊天内容标志
#define _offline_chat_flage     (0)
//离线保存，添加好友标记位
#define _offline_addFriend_flage (1)

//群聊邀请
#define _invite_success      (0)
#define _invite_refuse       (1)

//文件请回复
#define _file_accept (0)
#define _file_refuse (1)
//协议结构体
//注册请求
typedef struct STU_TCP_REGISTER_QS {
    STU_TCP_REGISTER_QS() :type(_DEF_TCP_REGISTER_QS) {
        memset(NickName, 0, sizeof(_DEF_MAX_LEN));
        memset(PassWord, 0, sizeof(_DEF_MAX_LEN));
        memset(Tel, 0, sizeof(_DEF_MAX_LEN));
    }
    int type;
    char NickName[_DEF_MAX_LEN];
    char PassWord[_DEF_MAX_LEN];
    char Tel[_DEF_MAX_LEN];
}STU_TCP_REGISTER_QS;
//注册回复
typedef struct STU_TCP_REGISTER_RE {
    STU_TCP_REGISTER_RE() :type(_DEF_TCP_REGISTER_RE), result(_register_name_repeat) {}
    int type;
    int result;
}STU_TCP_REGISTER_RE;
//登入请求
typedef struct STU_TCP_LOGIN_QS {
    STU_TCP_LOGIN_QS() :type(_DEF_TCP_LOGIN_QS) {
        memset(tel, 0, sizeof(_DEF_MAX_LEN));
        memset(PassWord, 0, sizeof(_DEF_MAX_LEN));
    }
    int type;
    char tel[_DEF_MAX_LEN];
    char PassWord[_DEF_MAX_LEN];
}STU_TCP_LOGIN_QS;
//登入回复
typedef struct STU_TCP_LOGIN_RE {
    STU_TCP_LOGIN_RE() :type(_DEF_TCP_LOGIN_RE), result(_login_password_error), user_id(-1) {
    }
    int type;
    int user_id;
    int result;
}STU_TCP_LOGIN_RE;
//聊天请求：聊天内容，好友id，自己id
typedef struct STU_TCP_CHAT_QS {
    STU_TCP_CHAT_QS() :type(_DEF_TCP_CHAT_QS), User_Id(-1), friend_id(-1) {
        memset(Content, 0, _DEF_CONTENT_LEN);
    }
    int type;
    int User_Id;
    int friend_id;
    char Content[_DEF_CONTENT_LEN];
}STU_TCP_CHAT_QS;
//聊天回复
typedef struct STU_TCP_CHAT_RE {
    STU_TCP_CHAT_RE() :type(_DEF_TCP_CHAT_RE), result(_chat_send_error), friend_id(-1) {
    }
    int type;
    int friend_id;
    int result;
}STU_TCP_CHAT_RE;
//添加好友请求:根据昵称添加好友，好友昵称，自己id，自己昵称
typedef struct STU_TCP_ADDFRIEND_QS {
    STU_TCP_ADDFRIEND_QS() :type(_DEF_TCP_ADD_FRIEND_QS), User_Id(-1) {
        memset(Friend_NickName, 0, sizeof(_DEF_MAX_LEN));
        memset(User_NickName, 0, sizeof(_DEF_MAX_LEN));
    }
    int type;
    char Friend_NickName[_DEF_MAX_LEN];
    char User_NickName[_DEF_MAX_LEN];
    int User_Id;
}STU_TCP_ADDFRIEND_QS;
//添加好友回复:好友昵称
typedef struct STU_TCP_ADDFRIEND_RE {
    STU_TCP_ADDFRIEND_RE() :type(_DEF_TCP_ADD_FRIEND_RE), result(_addfriend_refuse), friend_id(-1), user_id(-1) {
        memset(Friend_NickName, 0, sizeof(_DEF_MAX_LEN));
    }
    int type;
    char Friend_NickName[_DEF_MAX_LEN];
    int friend_id;
    int user_id;
    int result;
}STU_TCP_ADDFRIEND_RE;
//下线请求:用户id
typedef struct STU_TCP_OFFLINE_QS {
    STU_TCP_OFFLINE_QS() :type(_DEF_TCP_OFFLINE_QS), UserId(-1) {}
    int type;
    int UserId;
}STU_TCP_OFFLINE_QS;
//用户信息结构体
typedef struct STU_TCP_USERINFORMATION {
    STU_TCP_USERINFORMATION() :type(_DEF_TCP_USERINFORMATION), state(_user_state_offoline), user_id(-1) {
        memset(NickName, 0, _DEF_MAX_LEN);
        memset(Feeling, 0, _DEF_MAX_LEN);
    }
    int type;
    int state;
    int icon;
    char NickName[_DEF_MAX_LEN];
    char Feeling[_DEF_MAX_LEN];
    int user_id;
}STU_TCP_USERINFORMATION;
//离线消息
typedef struct STU_TCP_OFFLINECHAT{
    STU_TCP_OFFLINECHAT():type(_DEF_TCP_OFFLINECHAT),userId(-1),friendId(-1),time(0){
        memset(Content,0,_DEF_MAX_LEN);
    }
    int type;
    int userId;
    int friendId;
    time_t time;
    char Content[_DEF_MAX_LEN];
}STU_TCP_OFFLINECHAT;
//创建聊天群请求
typedef struct STU_TCP_CRE_GROUP_QS{
    STU_TCP_CRE_GROUP_QS():type(_DEF_TCP_CRE_GROUP_QS),userId(-1){
        memset(group_name,0,_DEF_MAX_LEN);
    }
    int type;
    int userId;
    char group_name[_DEF_MAX_LEN];
}STU_TCP_CRE_GROUP_QS;
//创建聊天群回复
typedef struct STU_TCP_CRE_GROUP_RE{
    STU_TCP_CRE_GROUP_RE():type(_DEF_TCP_CRE_GROUP_RE),owner(-1),group_id(-1){
        memset(group_name,0,_DEF_MAX_LEN);
        memset(group_announce,0,_DEF_MAX_LEN);
    }
    int     type;
    int     owner;
    int     group_id;
    char    group_announce[_DEF_MAX_LEN];
    char    group_name[_DEF_MAX_LEN];
}STU_TCP_CRE_GROUP_RE;
//群聊iteam，这里与用户iteam分开
typedef struct STU_TCP_GROUPINFORMATION {
    STU_TCP_GROUPINFORMATION() :type(_DEF_TCP_GROUPINFORMATION), GroupId(-1),Icon(0),owner(-1){
        memset(GroupName, 0, _DEF_MAX_LEN);
        memset(group_announce, 0, _DEF_MAX_LEN);
    }
    int     type;
    int     GroupId;
    char    GroupName[_DEF_MAX_LEN];
    char    group_announce[_DEF_MAX_LEN];
    int     owner;
    int     Icon;
}STU_TCP_GROUPINFORMATION;
//群聊消息请求
typedef struct STU_TCP_GROUPCHATQS {
    STU_TCP_GROUPCHATQS() :type(_DEF_TCP_GROUPCHATQS),groupId(-1),time(0),send_id(-1){
        memset(content,0,_DEF_MAX_LEN);
        memset(send_name,0,_DEF_MAX_LEN);
    }
    int     type;
    int     groupId;
    char    content[_DEF_MAX_LEN];
    time_t  time;
    char    send_name[_DEF_MAX_LEN];
    int     send_id;
}STU_TCP_GROUPCHATQS;
//群聊邀请请求
typedef struct STU_TCP_GROUPINVITEQS {
    STU_TCP_GROUPINVITEQS() :type(_DEF_TCP_GROUPINVITEQS),groupId(-1),time(0),send_id(-1){
        memset(send_name,0,_DEF_MAX_LEN);
        memset(recv_name,0,_DEF_MAX_LEN);
        memset(Group_name,0,_DEF_MAX_LEN);
    }
    int     type;
    int     groupId;
    time_t  time;
    char    send_name[_DEF_MAX_LEN];
    char    recv_name[_DEF_MAX_LEN];
    int     send_id;
    char    Group_name[_DEF_MAX_LEN];
}STU_TCP_GROUPINVITEQS;
//群聊邀请回复
typedef struct STU_TCP_GROUPINVITERE {
    STU_TCP_GROUPINVITERE() :type(_DEF_TCP_GROUPINVITERE),groupId(-1),recv_id(-1),send_id(-1),res(_invite_refuse){
        memset(send_name,0,_DEF_MAX_LEN);
        memset(recv_name,0,_DEF_MAX_LEN);
    }
    int     type;
    int     groupId;
    char    send_name[_DEF_MAX_LEN];
    int     send_id;
    int     recv_id;
    char    recv_name[_DEF_MAX_LEN];
    int     res;
}STU_TCP_GROUPINVITERE;
//文件信息请求
typedef struct STU_TCP_FILEINFO_QS {
    STU_TCP_FILEINFO_QS() :type(_DEF_TCP_FILEINFO_QS),recver_Id(0),sender_Id(0),size(0){
        memset(fileName,0,FILE_MAX_PATH);
        memset(fileId,  0,FILE_MAX_PATH);
    }
    int  type;
    int recver_Id;
    int sender_Id;
    char fileName[FILE_MAX_PATH];//文件名字
    char fileId  [FILE_MAX_PATH];//文件ID
    long long size;//文件大小
}STU_TCP_FILEINFO_QS;
//文件内容
typedef struct STU_TCP_FILEBLOCK_QS{
    STU_TCP_FILEBLOCK_QS() :type(_DEF_TCP_FILEBLOCK_QS),blockSize(0),recver_Id(0),sender_Id(0){
        memset(fileContent,0,FILE_CONTENT_SIZE);
        memset(fileId,  0,FILE_MAX_PATH);
    }
    int     type;
    int recver_Id;
    int sender_Id;
    char    fileContent[FILE_CONTENT_SIZE];//文件块内容
    char    fileId  [FILE_MAX_PATH];//文件Id
    int     blockSize;//块大小
}STU_TCP_FILEBLOCK_QS;


//文件信息回复
typedef struct STU_TCP_FILEINFO_RE {
    STU_TCP_FILEINFO_RE() :type(_DEF_TCP_FILEINFO_RE),res(_file_refuse),recver_Id(0),sender_Id(0)
    {
        memset(fileId  , 0,FILE_MAX_PATH);
    }
    int  type;
    int recver_Id;
    int sender_Id;
    char fileId  [FILE_MAX_PATH];//文件ID
    int  res;
}STU_TCP_FILEINFO_RE;
//文件信息结构体
typedef struct FILEINFO {
    FILEINFO():pFile(NULL),nFileSize(0){
        memset(fileName, 0,FILE_MAX_PATH);
        memset(fileId  , 0,FILE_MAX_PATH);
        memset(filePath, 0,FILE_MAX_PATH);
    }
    char fileId  [FILE_MAX_PATH];//文件ID
    char fileName[FILE_MAX_PATH];//文件名字
    char filePath[FILE_MAX_PATH];//文件路径
    FILE* pFile;        //文件指针
    long long nPos;     //当前在什么位置
    long long nFileSize;//文件总大小
}FILEINFO;
