#include <CKernal.h>
#include<TCPServerMediator.h>
#include<packDef.h>
#include<mysocket.h>
CKernal* CKernal::m_kernal=nullptr;
CKernal::CKernal()
{
	p_mMediator = new TCPServerMediator;
	m_kernal = this;
	//类成员函数指针与函数的绑定
	Deal_Protocl_Arr();
}
//类成员函数指针与函数的绑定
void CKernal::Deal_Protocl_Arr(){
	memset(Fun_PtrArr,0, sizeof(Fun_PtrArr));
	Fun_PtrArr[_DEF_TCP_REGISTER_QS     - _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_Register_QS;
	Fun_PtrArr[_DEF_TCP_LOGIN_QS        - _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_Login_QS;
	Fun_PtrArr[_DEF_TCP_OFFLINE_QS      - _DEF_PROTOCOL_BASE - 1] =&CKernal::Deal_Offline_QS;
	Fun_PtrArr[_DEF_TCP_CHAT_QS         - _DEF_PROTOCOL_BASE - 1] =&CKernal::Deal_Chat_QS;
	Fun_PtrArr[_DEF_TCP_ADD_FRIEND_QS   - _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_AddFriend_QS;
	Fun_PtrArr[_DEF_TCP_ADD_FRIEND_RE   - _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_AddFriend_RE;
	Fun_PtrArr[_DEF_TCP_CRE_GROUP_QS    - _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_CreGroup_QS;
	Fun_PtrArr[_DEF_TCP_GROUPCHATQS     - _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_GroupChatQs;
	Fun_PtrArr[_DEF_TCP_GROUPINVITEQS 	- _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_GroupInvite_QS;
	Fun_PtrArr[_DEF_TCP_GROUPINVITERE   - _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_GroupInvite_RE;
	Fun_PtrArr[_DEF_TCP_FILEINFO_QS     - _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_SendFileInfoQs;//发送文件信息请求
	Fun_PtrArr[_DEF_TCP_FILEINFO_RE     - _DEF_PROTOCOL_BASE - 1] = &CKernal::Deal_SendFileInfoRe;//发送文件信息请求
	Fun_PtrArr[_DEF_TCP_FILEBLOCK_QS     - _DEF_PROTOCOL_BASE - 1] =&CKernal::Deal_FileContent;//处理文件内容
}
CKernal::~CKernal()
{
	if (p_mMediator) {
		delete p_mMediator;
		p_mMediator = nullptr;
	}
}
bool CKernal::startNet()
{

	//连接数据库
	if (m_MySql.ConnectMySql("127.0.0.1", "root", "123456789", "IM")) {
		cout << "数据库连接成功" << endl;
	}else{
		cout << "数据库连接失败" << endl;
		return false;
	}
	if (!p_mMediator->openNet()) {
		return false;
	}
	return true;
}
bool CKernal::closeNet()
{
	return p_mMediator->closeNet();
}
void CKernal::dealData(char* data, int len, long from)
{
	int typ = *(int*)data;
	int index = typ - _DEF_PROTOCOL_BASE - 1;
	//判断下标的合法性
	if (0 <= index && index < _DEF_PROTOCOL_COUNT) {
		if (Fun_PtrArr[index]) {
			(this->*Fun_PtrArr[index])(data, len, from);
		}else {
			cout << __FUNCTION__ << ":" << "Fun_PtrArr[index]不存在" << endl;
		}
	}else{
		cout << __FUNCTION__<<":"<< "index 不合法" << endl;
	}
}
//注册请求
void CKernal::Deal_Register_QS(char* data, int len, long from) {
	STU_TCP_REGISTER_QS* reg = (STU_TCP_REGISTER_QS*)data;
	STU_TCP_REGISTER_RE reg_re;
	//校验数据合法性
	//查询电话是否存在
	list<string> res_lst;
	list<string> nickname_list;
	char Sql_Buf[1024];
	memset(Sql_Buf, 0, sizeof(Sql_Buf));
	sprintf(Sql_Buf,"select tel from t_user where tel=%s;",reg->Tel);
	if (m_MySql.SelectMySql(Sql_Buf, 1, res_lst)) {//如果查询成功
		if (res_lst.size() != 0){//电话号存在
			cout << "电话号存在" << endl;
			reg_re.result = _register_tel_repeat;
		}else{//如果电话号不存在
			//查看昵称是否存在
			sprintf(Sql_Buf, "select user_name from t_user where user_name='%s';", reg->NickName);
			if(m_MySql.SelectMySql(Sql_Buf, 1, nickname_list)){
				if (nickname_list.size() != 0){
					cout << "用户名已经存在" << endl;
					reg_re.result = _register_name_repeat;
				}else {

					sprintf(Sql_Buf, "insert into t_user (user_name,tel,user_password,feeling,icon_id)values('%s','%s','%s','edit you feeling',7);", 
							reg->NickName,reg->Tel,reg->PassWord);
					if (!m_MySql.UpdateMySql(Sql_Buf)) {
						cout << "插入失败:" << Sql_Buf << endl;
						return;
					}else{
						reg_re.result = _register_success;
						cout << "注册成功" << endl;
					}
				}

			}else{
				cout << "昵称查询失败:"<< Sql_Buf << endl;
				return ;
			}
		}
	}else{
		cout << "电话号查询失败:" << Sql_Buf<<endl;
		return;
	}
	p_mMediator->sendData((char*) & reg_re,sizeof(reg_re), from);
}
void CKernal::Deal_Login_QS(char* data, int len, long from) {
	STU_TCP_LOGIN_QS* qs = (STU_TCP_LOGIN_QS*)data;
	STU_TCP_LOGIN_RE re;
	//校验数据的合法性
	//根据电话号查询密码
	char Sql_buf[1024] = "";
	list<string> lst;
	sprintf(Sql_buf,"select user_password,id from t_user where tel='%s';",qs->tel);
	if (!m_MySql.SelectMySql(Sql_buf, 2, lst)) {
		cout << "查询失败:" << Sql_buf << endl;
		return;
	}
	if (lst.size() > 0){
		//查询到密码，校验密码的正确性
		string temp = lst.front();
		lst.pop_front();
		if (strcmp(temp.c_str(), qs->PassWord)==0) {
			//判断该用户是不是已经登入
			int user_id = stoi(lst.front());
			lst.pop_front();
			if (m_idToSock.find(user_id) != m_idToSock.end()) {
				//该用户已经登入
				re.result=_login_user_have_login;
			}else {
				//该用户没有登入，则登入成功
				re.result = _login_success;
				m_idToSock[user_id] = from;
				re.user_id = user_id;
				//得到用户信息以及好友信息
				p_mMediator->sendData((char*)&re, sizeof(re), from);
				//转发自信息已经好友iteam信息
				getUserInfor(user_id);
				//转发群iteam信息
				getGroupInfor(user_id);
				//转发该用户，离线时收到的消息
				getOfflineChat(user_id);
			}
		}else{
			cout << "密码错误" << endl;
			re.result = _login_password_error;
		}
	}else{
		//没有根据账号查询到密码,账号错误
		re.result = _login_no_this_user;
	}
	p_mMediator->sendData((char*)&re, sizeof(re), from);
}
//转发群iteam信息
void CKernal::getGroupInfor(int user_id){
	char sql_BUf[1024] = "";
	list<string> lst;
	STU_TCP_GROUPINFORMATION group_info; 
	sprintf(sql_BUf, "select Group_info.id, group_name,group_announcement,group_owner from Group_info inner join Group_member on Group_info.id=Group_member.id where Group_member.id_user=%d;",user_id);
	if (!m_MySql.SelectMySql(sql_BUf,4, lst)){
		cout << __FUNCTION__ << "查询失败:" << sql_BUf << endl;
		return ;
	}
	while(lst.size()!=0){
		int group_Id=stoi(lst.front());
		lst.pop_front();
		group_info.GroupId=group_Id;

		strcpy(group_info.GroupName,lst.front().c_str());
		lst.pop_front();

		strcpy(group_info.group_announce,lst.front().c_str());
		lst.pop_front();

		group_info.owner=stoi(lst.front());
		lst.pop_front();

		group_info.Icon=0;
		if(m_idToSock.find(user_id)!=m_idToSock.end()){
			p_mMediator->sendData((char*)&group_info,sizeof(group_info),m_idToSock[user_id]);
		}
	}
}
//转发该用户，离线时收到的消息
void CKernal::getOfflineChat(int user_id){
	STU_TCP_OFFLINECHAT offline_chat;
	STU_TCP_ADDFRIEND_QS add_qs;
	offline_chat.userId=user_id;
	//根据id获取聊天内容
	char sql_BUf[1024] = "";
	list<string> lst;
	sprintf(sql_BUf, "select Is_addFriend,idA,send_time,content,nameA,nameB from offline_chat where idB=%d order by send_time asc;",user_id);
	if (!m_MySql.SelectMySql(sql_BUf,6, lst)) {
		cout << __FUNCTION__ << "查询失败:" << sql_BUf << endl;
	}
	while(lst.size()!=0){
		int flage=stoi(lst.front());
		lst.pop_front();

		int sender_id=stoi(lst.front());
		lst.pop_front();

		time_t time=stoi(lst.front());
		lst.pop_front();

		string content=lst.front();
		lst.pop_front();

		string send_name=lst.front();
		lst.pop_front();

		string recver_name=lst.front();
		lst.pop_front();

		//如果是聊天请求
		if(flage==_offline_chat_flage){
			offline_chat.userId=sender_id;
			offline_chat.friendId=user_id;
			strcpy(offline_chat.Content,content.c_str());
			//转发给用户
			if(m_idToSock.find(user_id)!=m_idToSock.end()){
				p_mMediator->sendData((char*)&offline_chat,sizeof(offline_chat),m_idToSock[user_id]);
			}
		}else{
			//如果是添加好友请求
			strcpy(add_qs.Friend_NickName,recver_name.c_str());
			strcpy(add_qs.User_NickName,send_name.c_str());
			add_qs.User_Id=sender_id;
			if(m_idToSock.find(user_id)!=m_idToSock.end()){
				p_mMediator->sendData((char*)&add_qs,sizeof(add_qs),m_idToSock[user_id]);
			}
		}
	}
	//获取聊天内容之后把聊天内容从数据库中删除
	sprintf(sql_BUf,"delete from offline_chat where idB=%d;",user_id);
	if(!m_MySql.UpdateMySql(sql_BUf)){
		cout<<"删除失败"<<sql_BUf<<endl;	
	}
}
//得到用户信息以及好友信息
void CKernal::getUserInfor(int userId) {
	STU_TCP_USERINFORMATION user_infor;
	//根据id查询用户信息
	getInforByUserId(userId, &user_infor);
	//发给客户端
	if (m_idToSock.find(userId) != m_idToSock.end()) {
		p_mMediator->sendData((char*)&user_infor, sizeof(STU_TCP_USERINFORMATION), m_idToSock[userId]);
	}
	STU_TCP_USERINFORMATION friend_infor;
	//根据主用户id查询好友的用户id
	char sql_BUf[1024] = "";
	list<string> lst;
	sprintf(sql_BUf, "select idB from friend where idA=%d;", userId);
	if (!m_MySql.SelectMySql(sql_BUf,1,lst)) {
		cout << __FUNCTION__ << "查询失败:" << sql_BUf << endl;
		return;
	}
	//再根据好友的id得到好友的信息
	while (lst.size() > 0){
		int Id = stoi(lst.front());
		lst.pop_front();
		getInforByUserId(Id,&friend_infor);
		//发给客户端
		if (m_idToSock.find(userId) != m_idToSock.end()) {
			p_mMediator->sendData((char*)&friend_infor, sizeof(STU_TCP_USERINFORMATION), m_idToSock[userId]);
		}
		//把自己的信息发给好友一份，要不然好友的列表不会更新
		if (m_idToSock.find(Id) != m_idToSock.end()) {
			p_mMediator->sendData((char*)&user_infor, sizeof(STU_TCP_USERINFORMATION), m_idToSock[Id]);
		}
	}

}
//通过Id得到用户信息
void CKernal::getInforByUserId(int userId, STU_TCP_USERINFORMATION* user_infor) {
	char sql_BUf[1024] = "";
	list<string> lst;
	sprintf(sql_BUf, "select user_name,feeling,icon_id from t_user where id=%d;", userId);
	if (m_MySql.SelectMySql(sql_BUf, 3, lst)){
		if (lst.size() > 0) {
			string name = lst.front();
			lst.pop_front();
			strcpy(user_infor->NickName, name.c_str());

			string feeling = lst.front();
			lst.pop_front();
			strcpy(user_infor->Feeling, feeling.c_str());
			int temp_icon = stoi(lst.front());
			lst.pop_front();
			user_infor->icon = temp_icon;

			if (m_idToSock.find(userId) != m_idToSock.end()) {
				user_infor->state = _user_state_online;
			}else{
				user_infor->state = _user_state_offoline;
			}
			user_infor->user_id = userId;
		}
	}else{
		cout << __FUNCTION__ << ":查询错误" << sql_BUf << endl;
	}
}
//处理下线请求
void CKernal::Deal_Offline_QS(char* data, int len, long from) {
	cout << __FUNCTION__ << endl;
	STU_TCP_OFFLINE_QS* p_offline=(STU_TCP_OFFLINE_QS*)data;
	//发广播给该用户的好友
	//根据主用户id查询好友的用户id
	char sql_BUf[1024] = "";
	list<string> lst;
	sprintf(sql_BUf, "select idB from friend where idA=%d;", p_offline->UserId);
	if (!m_MySql.SelectMySql(sql_BUf, 1, lst)) {
		cout << __FUNCTION__ << "查询失败:" << sql_BUf << endl;
		return;
	}
	//循环遍历发给好友
	while (lst.size() > 0) {
		int Id = stoi(lst.front());
		lst.pop_front();
		//发给客户端
		//把自己的信息发给好友一份，要不然好友的列表不会更新
		if (m_idToSock.find(Id) != m_idToSock.end()) {
			p_mMediator->sendData((char*)data, sizeof(STU_TCP_OFFLINE_QS), m_idToSock[Id]);
		}
	}
	//关闭socket删除对应的idtosocket节点
	if (m_idToSock.find(p_offline->UserId) != m_idToSock.end()) {
		close(m_idToSock[p_offline->UserId]);
		auto v=m_idToSock.find(p_offline->UserId);
		m_idToSock.erase(v);
	}
}
//处理聊天请求
void CKernal::Deal_Chat_QS(char* data, int len, long from) {
	cout << __FUNCTION__ << endl;
	STU_TCP_CHAT_QS* chat_qs=(STU_TCP_CHAT_QS*)data;
	STU_TCP_CHAT_RE chat_re;
	//查看对端用户是否在线
	//在线就给对端转发聊天请求
	if (m_idToSock.find(chat_qs->friend_id) != m_idToSock.end()) {
		p_mMediator->sendData(data, len, m_idToSock[chat_qs->friend_id]);
	}else{
		//好友不在线,将聊天内容，保存到数据库中
		char sql_BUf[1024] = "";
		list<string> lst;
		time_t nowtime;
		time(&nowtime);
		sprintf(sql_BUf, "insert into offline_chat values (%d,%d,%ld,'%s',%d,'_','_');",chat_qs->User_Id,chat_qs->friend_id, nowtime,chat_qs->Content,_offline_chat_flage);
		if (m_MySql.UpdateMySql(sql_BUf) == false) {
			cout << "insert error : " << sql_BUf << endl;
		}
	}
}
//添加好友请求
void CKernal::Deal_AddFriend_QS(char* data, int len, long from) {
	cout << __FUNCTION__ << endl;
	STU_TCP_ADDFRIEND_QS* p_add = (STU_TCP_ADDFRIEND_QS*)data;
	//查询好友是否存在
	char sql_BUf[1024] = "";
	list<string> lst;
	sprintf(sql_BUf, "select id from t_user where user_name='%s';", p_add->Friend_NickName);
	if (!m_MySql.SelectMySql(sql_BUf, 1, lst)) {
		cout << __FUNCTION__ << "查询失败:" << sql_BUf << endl;
		return;
	}
	STU_TCP_ADDFRIEND_RE re;
	strcpy(re.Friend_NickName, p_add->Friend_NickName);
	if (lst.size()==0){
		//好友不存在
		re.result = _addfriend_not_exists;
		p_mMediator->sendData((char *) & re, sizeof(re),from);
	}else{
		//如果好友存在并且,在线转发给他
		int friend_id = stoi(lst.front());
		lst.pop_front();
		if (m_idToSock.find(friend_id) != m_idToSock.end()) {
			p_mMediator->sendData(data, len, m_idToSock[friend_id]);
		}
		else{
			//如果好友不在线，把数据保存到数据库中
			sprintf(sql_BUf, "insert into offline_chat (idA,idB,send_time,content,Is_addFriend,nameA,nameB) values(%d,%d,0,'_',%d,'%s','%s');", p_add->User_Id,friend_id,_offline_addFriend_flage,p_add->User_NickName,p_add->Friend_NickName);
			if(!m_MySql.UpdateMySql(sql_BUf)){
				cout<<__FUNCTION__<<"插入失败"<<sql_BUf<<endl;	
			}
		}
	}
}
//添加好友回复
void CKernal::Deal_AddFriend_RE(char* data, int len, long from) {
	STU_TCP_ADDFRIEND_RE* p_addrre = (STU_TCP_ADDFRIEND_RE*)data;
	//如果对端同意
	if (p_addrre->result == _addfriend_success) {
		//更新数据库
		char sql_BUf[1024] = "";
		list<string> lst;
		sprintf(sql_BUf, "insert into friend values(%d,%d);", p_addrre->friend_id, p_addrre->user_id);
		if (!m_MySql.UpdateMySql(sql_BUf)) {
			cout << __FUNCTION__ << "插入失败:" << sql_BUf << endl;
			return;
		}
		sprintf(sql_BUf, "insert into friend values(%d,%d);", p_addrre->user_id, p_addrre->friend_id);
		if (!m_MySql.UpdateMySql(sql_BUf)) {
			cout << __FUNCTION__ << "插入失败:" << sql_BUf << endl;
			return;
		}
		//更新双端好友列表
		getUserInfor(p_addrre->friend_id);
	}
	if (m_idToSock.find(p_addrre->user_id) != m_idToSock.end()) {
		p_mMediator->sendData(data, len, m_idToSock[p_addrre->user_id]);
	}
}

//处理创建群聊请求
void CKernal::Deal_CreGroup_QS(char* data, int len, long from) {
	cout<<__FUNCTION__<<endl;
	STU_TCP_CRE_GROUP_QS *cre_groupqs=(STU_TCP_CRE_GROUP_QS*)data;
	//将数据存入数据库
	//以下应该是一个原子操作____________________________________________________________________
	char sql_BUf[1024]="";
	list<string> lst;
	sprintf(sql_BUf, "insert into Group_info (group_name,group_owner) values ('%s',%d);",cre_groupqs->group_name,cre_groupqs->userId);
	if(m_MySql.UpdateMySql(sql_BUf) == false) {
		cout << "insert error : " << sql_BUf << endl;
	}	
	sprintf(sql_BUf, "SELECT MAX(id) FROM Group_info;");
	if(m_MySql.SelectMySql(sql_BUf,1,lst) == false) {
		cout << "select error : " <<sql_BUf<< endl;
	}
	int group_Id=stoi(lst.front());
	lst.pop_front();
	//把信息插入群成员表
	sprintf(sql_BUf, "insert into Group_member (id,id_user) values (%d,%d);",group_Id,cre_groupqs->userId);
	if(m_MySql.UpdateMySql(sql_BUf) == false) {
		cout << "insert error : " << sql_BUf << endl;
	}
	//以上应该是一个原子操作____________________________________________________________________
	//创建聊天群聊回复
	STU_TCP_CRE_GROUP_RE cre_groupre;
	cre_groupre.owner=cre_groupqs->userId;
	cre_groupre.group_id=group_Id;
	strcpy(cre_groupre.group_announce,"edit...");
	strcpy(cre_groupre.group_name,cre_groupqs->group_name);
	if(m_idToSock.find(cre_groupre.owner)!=m_idToSock.end()){
		p_mMediator->sendData((char *)&cre_groupre,sizeof(cre_groupre),m_idToSock[cre_groupre.owner]);
	}
}
//群聊消息请求
void CKernal::Deal_GroupChatQs(char* data, int len, long from){
	cout<<__FUNCTION__<<endl;
	STU_TCP_GROUPCHATQS *groupChatqs=(STU_TCP_GROUPCHATQS *)data;
	//转发给群内在线用户
	char sql_BUf[1024]="";
	list<string> lst;
	sprintf(sql_BUf, "select id_user from Group_member where id=%d;",groupChatqs->groupId);
	if(m_MySql.SelectMySql(sql_BUf,1,lst) == false) {
		cout << "select error : " << sql_BUf << endl;
		return ;
	}
	while(lst.size()>0){
		int userId=stoi(lst.front());
		lst.pop_front();
		if(userId!=groupChatqs->send_id){
			if(m_idToSock.find(userId)!=m_idToSock.end()){
				p_mMediator->sendData(data,len,m_idToSock[userId]);	
			}
		}
	}
}
//群聊邀请好友请求
void CKernal::Deal_GroupInvite_QS(char* data, int len, long from){	
	STU_TCP_GROUPINVITEQS* invite_sq=(STU_TCP_GROUPINVITEQS *)data;
	char sql_BUf[1024]="";
	list<string> lst;
	sprintf(sql_BUf, "select id from t_user where user_name='%s';",invite_sq->recv_name);
	if(m_MySql.SelectMySql(sql_BUf,1,lst) == false) {
		cout << "select error : " << sql_BUf << endl;
		return ;
	}
	int id=stoi(lst.front());
	lst.pop_front();
	if(m_idToSock.find(id)!=m_idToSock.end()){
		p_mMediator->sendData(data,len,m_idToSock[id]);	
	}
}
//群聊邀请好友回复
void CKernal::Deal_GroupInvite_RE(char* data, int len, long from){
	STU_TCP_GROUPINVITERE* groupInviteRe=(STU_TCP_GROUPINVITERE*) data;	
	char sql_BUf[1024]="";
	list<string> lst;
	if(groupInviteRe->res==_invite_success){
		//将好友写入群成员表
		sprintf(sql_BUf, "insert into Group_member values (%d,%d);",groupInviteRe->groupId,groupInviteRe->send_id);
		if(m_MySql.UpdateMySql(sql_BUf) == false) {
			cout << "insert error : " << sql_BUf << endl;
			return ;
		}
	}
	getGroupInfor(groupInviteRe->send_id);
	//通知群内的所有成员
	sprintf(sql_BUf, "select id_user from Group_member where id=%d;",groupInviteRe->groupId);
	if (!m_MySql.SelectMySql(sql_BUf, 1, lst)) {
		cout << __FUNCTION__ << "查询失败:" << sql_BUf << endl;
		return;
	}
	while(lst.size()>0){
		int id=stoi(lst.front());
		lst.pop_front();
		if(m_idToSock.find(id)!=m_idToSock.end()){
			p_mMediator->sendData(data,len,m_idToSock[id]);
		}
	}
}
//发送文件信息请求
void CKernal::Deal_SendFileInfoQs(char* data, int len, long from){
	//cout<<__FUNCTION__<<endl;
	STU_TCP_FILEINFO_QS* FileInfo_qs=(STU_TCP_FILEINFO_QS*)data;
	//判断对发是否在线
	if(m_idToSock.find(FileInfo_qs->recver_Id)!=m_idToSock.end()){
		//在线把文件信息请求发给对方
		p_mMediator->sendData(data,len,m_idToSock[FileInfo_qs->recver_Id]);	
	}else{//保存到数据库
	}
}
//发送文件信息回复
void CKernal::Deal_SendFileInfoRe(char* data, int len, long from){
	 cout<<__FUNCTION__<<endl;
	 STU_TCP_FILEINFO_RE* fileinfo_re=(STU_TCP_FILEINFO_RE*)data;
	 if(fileinfo_re->res==_file_accept){
		if(m_idToSock.find(fileinfo_re->recver_Id)!=m_idToSock.end()){
		    cout<<"2"<<endl;
			p_mMediator->sendData(data,len,m_idToSock[fileinfo_re->recver_Id]);
		}
	 }
}
//处理文件内容
void CKernal::Deal_FileContent(char* data, int len, long from){
	cout<<__FUNCTION__<<endl;
	STU_TCP_FILEBLOCK_QS* fileBlock=(STU_TCP_FILEBLOCK_QS*)data;
	//查看对端是否在线在先转发
	if(m_idToSock.find(fileBlock->recver_Id)!=m_idToSock.end()){
		p_mMediator->sendData(data,len,m_idToSock[fileBlock->recver_Id]);
	}
}
