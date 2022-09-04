#include "tcpfileserver.h"

TcpFileServer::TcpFileServer(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()),this, SLOT(acceptConnection()));
}

void TcpFileServer::startListen(QString ip, int port)
{

    QHostAddress hostip(ip);
    if(!tcpServer->listen(hostip,port)){
        qDebug()<<tcpServer->errorString();
    }else{
        qDebug()<<"正在监听";
    }

}

void TcpFileServer::tcpConnected()
{
    qDebug()<<"server tcp连接";
}

void TcpFileServer::tcpDisconnected()
{
    qDebug()<<"server tcp断开连接";
}

void TcpFileServer::displayError(QAbstractSocket::SocketError err)
{
    qDebug() << err;
}

void TcpFileServer::acceptConnection()
{
    qDebug()<<"222";
    //获得链接套接字
        tcpServerConnection = tcpServer->nextPendingConnection();

        //接收数据
        //readyRead()当网络套接字上有新的网络数据有效负载时
        connect(tcpServerConnection, SIGNAL(readyRead()),
                this, SLOT(updateServerProgress()));
        //处理异常
        connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayError(QAbstractSocket::SocketError)));

        // 关闭服务器，不再进行监听
//        tcpServer.close();
}

void TcpFileServer::updateServerProgress()
{
    QByteArray buffer;
    buffer = tcpServerConnection->readAll();
    QByteArray newData = unpackData + buffer;
    unpackData.clear();
    int countbuff = 0;
    qDebug()<<"buffer len:"<<buffer.size();

    if(newData.size()%TCP_PACKAGE_SIZE == 0)
    {
        if(newData.size()>TCP_PACKAGE_SIZE){
            do{
                handleByteData(newData.mid(countbuff,TCP_PACKAGE_SIZE));
                countbuff += TCP_PACKAGE_SIZE;
            }while(countbuff<newData.size());
        }else{
            qDebug()<<"数据<="<<TCP_PACKAGE_SIZE;
            handleByteData(newData);
        }
    }else{
        int unpackLen = newData.size() - newData.size()%TCP_PACKAGE_SIZE;
        QByteArray tempData = newData.mid(0,unpackLen);
        unpackData = newData.mid(unpackLen,newData.size()%TCP_PACKAGE_SIZE);
        if(newData.size()>=TCP_PACKAGE_SIZE){
            do{
                handleByteData(tempData.mid(countbuff,TCP_PACKAGE_SIZE));
                countbuff += TCP_PACKAGE_SIZE;
            }while(countbuff<tempData.size());
        }
    }


}

void TcpFileServer::handleByteData(QByteArray data)
{
    //前100为文件头
    quint16 cmdid1 = streamuint16(data.left(2));//2位:1第一条数据，2中间数据，3最后一条数据
    quint32 cmdid2 = streamuint32(data.mid(2,4));//4位:1文本，2图片，3文件，
    quint32 cmdid3 = streamuint32(data.mid(6,4));//4位:计数
    quint32 cmdid4 = streamuint32(data.mid(10,4));//4位:此条数据中有效内容
    quint32 cmdid5 = streamuint32(data.mid(14,4));//4位:文件总大小
    quint16 cmdid6 = streamuint16(data.mid(18,2));//2位:识别码长度
    QByteArray cmd7 = data.mid(20,cmdid6);  //识别码,uuid 38字符
    QByteArray imgdata = data.mid(DATA_HEAD_SIZE,cmdid4);

//    qDebug()<<"i:"<<cmdid3<<",isImage uuid:"<<cmd7<<",fileLen:"<<imgdata.size();

    fileData += imgdata;
    emit ReceivingDataProgress(fileData.size(),cmdid5);

    if(cmdid1 == 3)
    {
        qDebug()<<"end:3";
        emit resultData(fileData,cmdid2);
        fileData.clear();
    }
}

void TcpFileServer::closeServer()
{
    if(tcpServer != NULL)
        tcpServer->close();
}

quint16 TcpFileServer::streamuint16(QByteArray data)
{
    QDataStream stream(data);
    quint16 d2;
    stream>>d2;
    return d2;
}

quint32 TcpFileServer::streamuint32(QByteArray data)
{
    QDataStream stream(data);
    quint32 d2;
    stream >> d2;
    return d2;
}

QImage TcpFileServer::get_imagedata_from_byte(const QString &data)
{
    QByteArray imageData = QByteArray::fromBase64(data.toLatin1());
    QImage image;
    image.loadFromData(imageData);
    return image;
}

QByteArray TcpFileServer::get_imagedata_from_imagefile(const QImage &image)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    image.save(&buffer, "jpg");
    imageData = imageData.toBase64();
    return imageData;
}
