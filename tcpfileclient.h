#ifndef TCPFILECLIENT_H
#define TCPFILECLIENT_H

#include <QObject>
#include <tcpfileserver.h>
#include <QBuffer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QImage>
#include <QPixmap>
#include <iostream>
#include <QUuid>
#include <QByteArray>

//文件传输客户端 -- 用于发送
class TcpFileClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpFileClient(QObject *parent = nullptr);
    QByteArray get_imagedata_from_imagefile(const QImage &image);
    QImage get_imagedata_from_byte(const QString &data);
    void connectServer(QString ip,int port);
    void sendData(QByteArray data, int dataType);
    void closeClient();

signals:
    void sendDataProgress(int sendSize,int fileSize);   //发送文件进度,发送大小,文件大小
    void resultSend();//完成发送

private slots:
    void tcpConnected();
    void tcpDisconnected();
    void displayError(QAbstractSocket::SocketError);

private:
    QTcpSocket *tcpclient;

};

#endif // TCPFILECLIENT_H
