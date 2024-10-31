#ifndef GROUPITEAM_H
#define GROUPITEAM_H

#include <QWidget>

namespace Ui {
class groupiteam;
}

class groupiteam : public QWidget
{
    Q_OBJECT

public:
    explicit groupiteam(QWidget *parent = nullptr);
    ~groupiteam();
    void setInFor(int GroupId,QString GroupName,QString Announce,int Icon,int GroupOwner);
signals:
    void SIG_showGroupChat(int id);
private slots:
    void on_pb_Icon_clicked();
private:
    Ui::groupiteam *ui;
    int m_GroupId;
    QString m_GroupName;
    QString m_Announce;//群公共
    int m_GroupOwner;//群主
    int m_Icon;
};

#endif // GROUPITEAM_H
