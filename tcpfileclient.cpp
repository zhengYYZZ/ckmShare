#include "tcpfileclient.h"

TcpFileClient::TcpFileClient(QObject *parent) : QObject(parent)
{
    tcpclient = new QTcpSocket(this);

}

void TcpFileClient::connectServer(QString ip, int port)
{
    connect(tcpclient,SIGNAL(connected()), this, SLOT(tcpConnected()));
    connect(tcpclient, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
    connect(tcpclient, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
    tcpclient->connectToHost(ip,port);
}

void TcpFileClient::sendData(QByteArray data,int dataType)
{
    int countdata = 0;
    int sendDataSize = 0;
    int i=0;
    do{
        QByteArray dataPackage;
        QDataStream dataTial(&dataPackage,QIODevice::ReadWrite);
        QByteArray dataTemp = data.mid(countdata,DATA_VALID_SIZE);
        QUuid uuid = QUuid::createUuid();
        QByteArray byteUuid = uuid.toByteArray();
        qDebug()<<byteUuid;

        //前1000为文件头
        //2位:1第一条数据，2中间数据，3最后一条数据
        //4位:1图片，2文件，3文本
        //4位:计数
        //4位:此条数据中有效内容
        //4位:文件总大小
        //2位:识别码长度
        //128位:识别码，暂定为uuid
        //DATA_HEAD_SIZE位后:数据
        dataTial << quint32(dataType); //4位:1图片，2文件，3文本
        dataTial << quint32(i); //4位:计数
        dataTial << quint32(dataTemp.size());   //4位:此条数据中有效内容
        dataTial << quint32(data.size());  //4位:文件总大小
        dataTial << quint16(byteUuid.size());   //2位:识别码长度
        dataPackage = dataPackage.insert(20,byteUuid.data(),byteUuid.size());//128位:识别码，暂定为uuid
        dataPackage = dataPackage.insert(DATA_HEAD_SIZE,dataTemp.data(),dataTemp.size());//1000后:数据
        dataPackage.resize(4096);
        tcpclient->write(dataPackage);

        sendDataSize += dataTemp.size();
        countdata += DATA_VALID_SIZE;
        i+=1;
        emit sendDataProgress(sendDataSize,data.size());
    }while(countdata < data.size());
    emit resultSend();
}


QByteArray TcpFileClient::get_imagedata_from_imagefile(const QImage &image)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    image.save(&buffer, "jpg");
    imageData = imageData.toBase64();
    return imageData;
}


QImage TcpFileClient::get_imagedata_from_byte(const QString &data)
{
    QByteArray imageData = QByteArray::fromBase64(data.toLatin1());
    QImage image;
    image.loadFromData(imageData);
    return image;
}

void TcpFileClient::tcpConnected()
{
    qDebug()<<"tcp连接";
}

void TcpFileClient::tcpDisconnected()
{
    qDebug()<<"tcp断开连接";
}

void TcpFileClient::displayError(QAbstractSocket::SocketError err)
{
    qDebug() << err;
}

void TcpFileClient::closeClient()
{
    if(tcpclient != NULL)
        tcpclient->close();
}
