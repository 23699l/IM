#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class chatDialog; }
QT_END_NAMESPACE

class chatDialog : public QDialog
{
    Q_OBJECT

public:
    chatDialog(QWidget *parent = nullptr);
    ~chatDialog();
protected:
    void closeEvent(QCloseEvent *);
private slots:
    void on_pb_register_clicked();

    void on_pb_clear_register_clicked();

    void on_pb_clear_clicked();

    void on_pb_login_clicked();

private:
    Ui::chatDialog *ui;
signals:
    void SIG_RegisterMessage(QString tel,QString password,QString nickname);
    void SIG_LoginMessage(QString tel,QString password);
    void SIG_closeApp();//关闭登入注册界面
};
#endif // CHATDIALOG_H
