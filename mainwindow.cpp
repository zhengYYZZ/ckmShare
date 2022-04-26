#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("cmkShare");

    my_port = 6666;
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any,my_port);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(readUdpText()));

    heartTimer = new QTimer(this);
    connect(heartTimer,SIGNAL(timeout()),this,SLOT(heartbeatTimer()));
    heartTimer->start(1000);

    read_local_ip();    //获取本地ip
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::read_local_ip()
{
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    for(int i=0; i<ipList.size(); i++)
    {
        if(ipList.at(i) != QHostAddress::LocalHost && ipList.at(i).toIPv4Address())
        {
            ui->localIPcomboBox->addItem(ipList.at(i).toString());
        }
    }
}

void MainWindow::read_local_ini(QString path)
{

}

/**********************************
* 函数名:  concet_send
* 函数描述: 发起连接请求
* 输入参数:
*       ip: 目标ip
*       port: 数据传输端口，不是udpSocket绑定的端口
* 输出参数:
* 返回值:
************************************/
void MainWindow::concet_send(QString ip, int port)
{
    QString localIp = ui->localIPcomboBox->currentText();
    QString sendstr = QString("ckmshare;connect;%1,%2").arg(localIp).arg(port);
    udpSocket->writeDatagram(sendstr.toUtf8(),QHostAddress(ip),my_port);
    qDebug()<<"sendConect:"<<sendstr;
}

void MainWindow::udpHandle(QString msg)
{
    QStringList strlist = msg.split(";");
    if(strlist.size()<=3)
        return;
    if(strlist.at(0) != "ckmshare")
        return;

    QString ip;
    if(strlist.at(1) == "connect")
    {
        //回应建立连接
        ip = strlist.at(2);
    }else if (strlist.at(1) == "heartbeat"){
        //回应心跳
        ip = strlist.at(2);
    }else if(strlist.at(1) == "heartheat2"){
        //心跳回应
        ip = strlist.at(2);
    }else if(strlist.at(1) == "text"){
        //文本粘贴内容
        ip = strlist.at(2);
        m_clipboardType = 1;
        m_clipData = strlist.at(3);
    }else if(strlist.at(1) == "image"){
        //传输图片x
        ip = strlist.at(2);
        m_clipboardType = 2;
    }else if(strlist.at(1) == "file"){
        //传输文件x
        ip = strlist.at(2);
        m_clipboardType = 3;
    }
}

void MainWindow::readUdpText()
{
    QByteArray arrdata;
    QString str;
    arrdata.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(arrdata.data(),arrdata.size());
    str.prepend(arrdata);
    qDebug()<<"readUdp:"<<str;
}

void MainWindow::heartbeatTimer()
{
    QString localIp = ui->localIPcomboBox->currentText();
    QString sendstr = QString("ckmshare;heartberat;%1").arg(localIp);

    QMap<QString,int>::const_iterator iter = m_serverMap.constBegin();
    while (iter != m_serverMap.constEnd()) {
        QString ip = iter.key();
        udpSocket->writeDatagram(sendstr.toUtf8(),QHostAddress(ip),my_port);
    }
    qDebug()<<"heartbeatTimer:"<<sendstr;
}
