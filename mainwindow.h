#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QList>
#include <QMap>
#include <QByteArray>
#include <QStringList>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readUdpText();
    void heartbeatTimer();  //定时器槽，心跳消息

public:
    void read_local_ip();   //读取本地ip
    void read_local_ini(QString path);  //读取ini配置文件
    void concet_send(QString ip,int port);     //发起连接请求
    void udpHandle(QString msg);

private:
    Ui::MainWindow *ui;

    //key：ip,val:port
    QMap<QString,int> m_clientMap;  //连接过来的ip列表
    QMap<QString,int> m_serverMap;  //连接出去的ip

    QUdpSocket *udpSocket;
    int my_port;
    QTimer *heartTimer;

    //粘贴类型
    int m_clipboardType;
    QString m_clipData;
};
#endif // MAINWINDOW_H
