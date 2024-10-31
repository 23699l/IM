#include "chat.h"
#include "ui_chat.h"
#include<QDebug>
#include<QTime>
#include<QMessageBox>
#include"./inet/packDef.h"
#include<QFileDialog>
#include<string>
using namespace std;
Chat::Chat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::setChat(int id, QString name)
{
    m_id=id;
    m_name=name;
    setWindowTitle(m_name);
}
//设置聊天窗口内容
void Chat::setChatContent(QString Content,::time_t time)
{
    //显示时间,并将文本显示到聊天窗口中
    if(time==0){
        ui->tb_chat->append(QString("%1:%2").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss")));
        ui->tb_chat->append(Content);
    }else{
        tm *p=::localtime(&time);
        char tm[30]="";
        sprintf(tm,"%04d:%02d:%02d %02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
        ui->tb_chat->append(QString("%1:%2").arg(m_name).arg(tm));
        ui->tb_chat->append(Content);
    }
}

void Chat::setChatRe(int state)//设置聊天回复
{

    if(state==_chat_send_error){
        ui->tb_chat->append(QString("对方离线:%1").arg(QTime::currentTime().toString("hh:mm:ss")));
    }
}

bool Chat::show_FileInfoQs(string fileName,int size)
{
    char content[1024]="";
    sprintf(content,"文件：%s,大小：%d字节",fileName.c_str(),size);
    if(QMessageBox::question(this,"提示",content)==QMessageBox::Yes){
        return true;
    }else{
        return false;
    }
}

//点击发送的槽
void Chat::on_pb_send_clicked()
{
    //获取文本
    QString text=ui->te_chat->toPlainText();//获取纯文本
    //QString text=ui->te_chat->toHtml();//获取html文本
    //清空编辑区
    ui->te_chat->setText("");
    //判断发送的内容是否为空
    QString text_temp=text;
    text_temp.remove(" ");
    if(text_temp.isEmpty()){
        QMessageBox::about(this,"警告","发送内容不能为空");
        return;
    }
    //显示时间,并将文本显示到聊天窗口中
    ui->tb_chat->append(QString("me:%1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(text);
    //发送给ckernal
    Q_EMIT SIG_SendContent(text,m_id);
}
string GetFileName(const char * filePath){
    if(filePath==nullptr){
        return string();
    }
    int len=strlen(filePath);
    if(len<=0){
        return string();
    }
    for(int i=len-1;i>=0;i--){
        if(filePath[i]=='\\'||filePath[i]=='/'){
            return &filePath[i+1];
        }
    }
    return string();
}
//发送文件
void Chat::on_pb_sandFile_clicked()
{
    QString path=QFileDialog::getOpenFileName(this,"open file","E:/",
                                              "所有文件(*.*);;");
    if(path.isEmpty()){
        return ;
    }
    qDebug()<<path;
    //发送文件请求
    string FilenName=GetFileName(path.toStdString().c_str());//文件名称
    qDebug()<<FilenName.c_str();
    time_t nowtime;
    struct tm * timeinfo;
    time(&nowtime);
    timeinfo = localtime(&nowtime);
    int current_hour = timeinfo->tm_hour;
    int current_minute = timeinfo->tm_min;
    int current_second = timeinfo->tm_sec;
     QFile file(path);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        // 处理错误，例如打印错误信息
        qDebug() << "无法打开文件：" << file.errorString();
        return;
    }else{
        qDebug()<<"成功打开文件:"<<path;
    }


//    FILE* p_File=NULL;
//    int erro=fopen_s(&p_File,path.toStdString().c_str(),"rb");
//    if(erro!=0){
//        qDebug()<<__FUNCTION__<<"fopen_s erro";
//    }
//    fseek(p_File,0,SEEK_END);
//    int fileSize=_ftelli64(p_File);//文件大小
//    fseek(p_File,0,SEEK_SET);
//    fclose(p_File);
    //Q_EMIT SIG_SendFileInfoQs(FilenName,current_hour,current_minute,current_second,/*fileSize*/0,m_id,p_File,path.toStdString());
}
