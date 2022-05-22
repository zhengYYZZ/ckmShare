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
#include <QStandardItemModel>

#include "connectdialog.h"
#include "setdialog.h"
#include "showconnectipcard.h"


/***
 * todo
 * 建立连接->ok
 * 心跳-->基本完成->ok
 * 断线重连-ok
 * 显示连接ip--ok
 * 保存配置INI
*/


struct addressHost
{
    QString ip;     //IP地址
    int port;       //端口号
    bool isConnect; //连接状态
    uint errCount;  //心跳错误计数
};

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
    void readUdpText();     //读取udp内容
    void heartbeatTimer();  //定时器槽，心跳消息
    void showConnectDlg();
    void showSetDlg();

public:
    void read_local_ip();                       //读取本地ip
    void read_local_ini(QString path);          //读取ini配置文件
    void concet_send(QString ip,int port);      //发起连接请求
    void concet_respond(QString ip,int port);   //回应连接请求
    void udpHandle(QString msg);                //处理接收到的信息
    void heartbeat_respond(QString ip);         //心跳消息处理
    void heartbeat2_respond(QString ip);        //心跳回应消息处理

private:
    Ui::MainWindow *ui;

    //key：ip,val:addressHost
    QMap<QString,addressHost> m_clientMap;  //连接过来的ip列表
    QMap<QString,addressHost> m_serverMap;  //连接出去的ip
    //用于表格显示
    QStandardItemModel *m_clientModel;
    QStandardItemModel *m_serverModel;

    QUdpSocket *udpSocket;
    int my_port;
    QTimer *heartTimer;

    //粘贴类型
    int m_clipboardType;
    QString m_clipData;
};
#endif // MAINWINDOW_H
