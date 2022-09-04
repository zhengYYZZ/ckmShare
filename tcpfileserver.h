#ifndef TCPFILESERVER_H
#define TCPFILESERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QBuffer>
#include <QPixmap>
#include <QImage>
#include <QDebug>
#include <QTcpServer>
#include <QTextCodec>
#include <QMap>

#define TCP_PACKAGE_SIZE 4096   //tcp一次发送的包大小
#define DATA_VALID_SIZE 3072    //包中有效数据大小
#define DATA_HEAD_SIZE 100      //包头大小


//文件传输服务端 -- 用于接收
class TcpFileServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpFileServer(QObject *parent = nullptr);
    void startListen(QString ip,int port);  //开始监听
    void closeServer(); //关闭
    quint16 streamuint16(QByteArray data);
    quint32 streamuint32(QByteArray data);
    QImage get_imagedata_from_byte(const QString &data);
    QByteArray get_imagedata_from_imagefile(const QImage &image);


signals:
    void resultData(QByteArray data,int dataType);
    void ReceivingDataProgress(int ReceivSize,int fileSize);   //发送文件进度,发送大小,文件大小

private slots:
    void tcpConnected();
    void tcpDisconnected();
    void displayError(QAbstractSocket::SocketError);
    void acceptConnection();//被客户端连接上后，创建套接字、接收数据、处理异常、关闭服务器
    void updateServerProgress();

private:
    void handleByteData(QByteArray data);

private:
    QTcpServer *tcpServer;
    QTcpSocket *tcpServerConnection;
    QByteArray fileData;
    QByteArray unpackData;  //拆包数据
};

#endif // TCPFILESERVER_H
