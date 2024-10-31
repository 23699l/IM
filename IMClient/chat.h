#ifndef CHAT_H
#define CHAT_H

#include <QDialog>
#include<stdio.h>
#include<ctime>
#include<string>
#include<map>
#include"fileiteam.h"
using namespace std;
namespace Ui {
class Chat;
}

class Chat : public QDialog
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();
    void setChat(int id,QString name);
    void setChatContent(QString Content,::time_t time=0);//设置聊天窗口内容
    void setChatRe(int state);//设置聊天回复
    bool show_FileInfoQs(string ,int);
private slots:
    void on_pb_send_clicked();
    void on_pb_sandFile_clicked();
private:
    Ui::Chat *ui;
    int m_id;
    QString m_name;

signals:
    void SIG_SendContent(QString content,int friend_id);
    void SIG_SendFileInfoQs(string,int,int,int,int,int,FILE*,string);
};

#endif // CHAT_H
