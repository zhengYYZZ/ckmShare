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
#include <QClipboard>
#include <QPixmap>
#include <QMimeData>
#include <QDateTime>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QThread>

#include "connectdialog.h"
#include "setdialog.h"
#include "tcpfileclient.h"
#include "tcpfileserver.h"
#include "exitdialog.h"

//test
#include "dialogtesttcp.h"
#include "dialogtcptest.h"


/***
 * todo
 * 建立连接->ok
 * 心跳->ok
 * 断线重连-ok
 * 显示连接ip--ok
 * 保存配置INI-->ok
 * 粘贴版-->todo
 *      - 文字--ok
 *          --多行带(;)文本需测试
 *      - 图片
 *      - 文件
 * 托盘最小化-->todo
 * 日志-->todo
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
    void closeEvent(QCloseEvent *e) override;               //关闭事件
    void setTaryIcon();                                     //设置系统托盘
    void updataSystemIni();                                 //获取更新./SYSTEM.INI文件信息

private slots:
    void readUdpText();     //读取udp内容
    void heartbeatTimer();  //定时器槽，心跳消息
    void showConnectDlg();
    void showSetDlg();
    void clipboardChanged();
    void tcpClientResult();
    void tcpClientSendProgress(int sendSize,int fileSize);
    void tcpServerResult(QByteArray data,int type);
    void tcpServerProgress(int ReceivSzie,int fileSize);
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason); //托盘响应函数

    void on_localIPcomboBox_currentIndexChanged(const QString &arg1);

    void on_actionslkdj_triggered();

    void on_actionserver_triggered();

public:
    void read_local_ip();                       //读取本地ip
    void concet_send(QString ip,int port);      //发起连接请求
    void concet_respond(QString ip,int port);   //回应连接请求
    void udpHandle(QString msg);                //处理接收到的信息
    void heartbeat_respond(QString ip);         //心跳消息处理
    void heartbeat2_respond(QString ip);        //心跳回应消息处理
    void get_clipboard_data();                  //获取粘贴板内容
    void send_clipboard_data(QString ip);       //发送粘贴板内容
    void set_clipboard_data(QString text);      //设置粘贴板内容 text
    QImage get_imagedata_from_byte(const QString &data);
    QByteArray get_imagedata_from_imagefile(const QImage &image);

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
    int tcp_port = 6667;       //文件传输端口
    int m_fileSize;     //文件传输大小上限
    QString m_tempFilePath; //文件传输临时存放位置
    QTimer *heartTimer;

    //粘贴类型
    int m_clipboardType = 0;      // 1：文字、2：图片、3：文件
    QString m_clipData;
    QPixmap m_clipImg;
    bool isLocalChangeClip = false;     //是否因设置引起粘贴板的改变
    TcpFileClient *tcpClient;
    TcpFileServer *tcpServer;
    QByteArray fileData;
    qint64 m_tempsystime=0;
    QSystemTrayIcon *m_sysTrayIcon;     //用于最小化到托盘


};
#endif // MAINWINDOW_H
