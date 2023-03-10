#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    tcpServer = new TcpFileServer();
    ui->setupUi(this);
    this->setWindowTitle("cmkShare");

    updataSystemIni();
    my_port = 6666;
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any,my_port);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(readUdpText()));

    heartTimer = new QTimer(this);
    connect(heartTimer,SIGNAL(timeout()),this,SLOT(heartbeatTimer()));
    heartTimer->start(1000);

    //action
    connect(ui->actiona,SIGNAL(triggered()),this,SLOT(showConnectDlg()));   //建立连接dlg
    connect(ui->actionafd,SIGNAL(triggered()),this,SLOT(showSetDlg()));     //设置dlg

    read_local_ip();    //获取本地ip

    //显示连接过来的IP列表
    m_clientModel = new QStandardItemModel();
    m_clientModel->setColumnCount(3);   //3列
    m_clientModel->setHeaderData(0,Qt::Horizontal,"IP地址");
    m_clientModel->setHeaderData(1,Qt::Horizontal,"端口号");
    m_clientModel->setHeaderData(2,Qt::Horizontal,"连接状态");
    m_clientModel->setRowCount(5);
    //设置列宽在可视界面自适应宽度
    ui->clientTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //隐藏行表头
    ui->clientTableView->verticalHeader()->setVisible(false);
    //行颜色交替显示
    ui->clientTableView->setAlternatingRowColors(true);
    //不允许在图形界面修改内容
    ui->clientTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->clientTableView->setModel(m_clientModel);
    ui->clientTableView->show();

    //显示连接出去的ip列表
    m_serverModel = new QStandardItemModel();
    m_serverModel->setColumnCount(3);   //3列
    m_serverModel->setHeaderData(0,Qt::Horizontal,"IP地址");
    m_serverModel->setHeaderData(1,Qt::Horizontal,"端口号");
    m_serverModel->setHeaderData(2,Qt::Horizontal,"连接状态");
    m_serverModel->setRowCount(5);
    //设置列宽在可视界面自适应宽度
    ui->serverTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //隐藏行表头
    ui->serverTableView->verticalHeader()->setVisible(false);
    //行颜色交替显示
    ui->serverTableView->setAlternatingRowColors(true);
    //不允许在图形界面修改内容
    ui->serverTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->serverTableView->setModel(m_serverModel);
    ui->serverTableView->show();

    connect(QApplication::clipboard(), SIGNAL(dataChanged()),this, SLOT(clipboardChanged()));

    tcpClient = new TcpFileClient();
    connect(tcpClient,SIGNAL(resultSend()),this,SLOT(tcpClientResult()));
    connect(tcpClient,SIGNAL(sendDataProgress(int,int)),this,SLOT(tcpClientSendProgress(int,int)));

//    QString localIp = ui->localIPcomboBox->currentText();
//    on_localIPcomboBox_currentIndexChanged(localIp);

    connect(tcpServer,SIGNAL(resultData(QByteArray,int)),this,SLOT(tcpServerResult(QByteArray,int)));
//    connect(tcpServer,SIGNAL(ReceivingDataProgress(int,int)),this,SLOT(tcpServerProgress(int,int)));
//    tcpServer->startListen(localIp,tcp_port);


    m_sysTrayIcon = new QSystemTrayIcon(this);
    connect(m_sysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::read_local_ip()
{
    //读取本地ip
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    for(int i=0; i<ipList.size(); i++)
    {
        if(ipList.at(i) != QHostAddress::LocalHost && ipList.at(i).toIPv4Address())
        {
            ui->localIPcomboBox->addItem(ipList.at(i).toString());
        }
    }
}

//连接 ==> 槽
void MainWindow::showConnectDlg()
{
    connectDialog dlg;
    dlg.exec();
    if(dlg.is_ok())
    {
        addressHost ah;
        ah.ip = dlg.get_ip();
        ah.isConnect = false;
        ah.errCount = 0;
        m_serverMap.insert(ah.ip,ah);
        concet_send(ah.ip,tcp_port);
        updataSystemIni();
        QString localIp = ui->localIPcomboBox->currentText();
        on_localIPcomboBox_currentIndexChanged(localIp);
    }
}

void MainWindow::showSetDlg()
{
    setDialog dlg;
    dlg.exec();
}


/**********************************
* 函数名:  concet_send
* 函数描述: 发起连接请求
* 输入参数:
*       ip: 目标ip
*       port: 本机数据传输端口，不是udpSocket绑定的端口
* 输出参数:
* 返回值:
************************************/
void MainWindow::concet_send(QString ip, int port)
{
    QString localIp = ui->localIPcomboBox->currentText();
    QString sendstr = QString("ckmshare;connect;%1;%2").arg(localIp).arg(port);
    udpSocket->writeDatagram(sendstr.toUtf8(),QHostAddress(ip),my_port);
    qDebug()<<"concet_send发起连接请求:"<<sendstr;
}

void MainWindow::concet_respond(QString ip, int port)
{
    QString localIp = ui->localIPcomboBox->currentText();
    QString sendstr = QString("ckmshare;connect2;%1;%2").arg(localIp).arg(port);
    udpSocket->writeDatagram(sendstr.toUtf8(),QHostAddress(ip),my_port);
//    qDebug()<<"concet_respond回应连接请求:"<<sendstr;
}

void MainWindow::udpHandle(QString msg)
{
    //消息处理
    QStringList strlist = msg.split(";");
    if(strlist.size()<3)
        return;
    if(strlist.at(0) != "ckmshare")
        return;

    QString ip;
    if(strlist.at(1) == "connect")
    {
        //处理建立连接
        ip = strlist.at(2);
        int port = strlist.at(3).toInt();
        addressHost ah;
        ah.ip = ip;
        ah.port = port;
        ah.isConnect = true;
        ah.errCount = 0;
        m_clientMap[ip] = ah;
        concet_respond(ip,tcp_port);    //回应
    }else if (strlist.at(1) == "connect2") {
        //处理连接回应
        ip = strlist.at(2);
        int port = strlist.at(3).toInt();
        if(m_serverMap[ip].ip == "")
            m_serverMap[ip].ip = ip;
        m_serverMap[ip].isConnect = true;
        m_serverMap[ip].port = port;
        m_serverMap[ip].errCount = 0;
    }else if (strlist.at(1) == "heartbeat"){
        //心跳处理
        ip = strlist.at(2);
        heartbeat_respond(ip);
    }else if(strlist.at(1) == "heartbeat2"){
        //心跳回应处理
        ip = strlist.at(2);
        heartbeat2_respond(ip);
    }else if(strlist.at(1) == "text"){
        //文本粘贴内容
        ip = strlist.at(2);
        QString tempstr;
        for(int i=3;i<strlist.length();i++){
            tempstr = tempstr + strlist.at(i);
        }
        set_clipboard_data(tempstr);
    }else if(strlist.at(1) == "image"){
        //传输图片x
        ip = strlist.at(2);

    }else if(strlist.at(1) == "file"){
        //传输文件x
        ip = strlist.at(2);
    }
}

//读取udp消息
void MainWindow::readUdpText()
{
    QByteArray arrdata;
    QString str;
    arrdata.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(arrdata.data(),arrdata.size());
    str.prepend(arrdata);
//    if(!str.contains("heartbeat",Qt::CaseSensitive))
//        qDebug()<<"readUdp:"<<str;
    qDebug()<<"readUdp:"<<str;
    udpHandle(str);
}

//发送心跳一秒一次
void MainWindow::heartbeatTimer()
{
    QString localIp = ui->localIPcomboBox->currentText();
    QString sendstr = QString("ckmshare;heartbeat;%1").arg(localIp);

    //检查是否有连接断开m_serverMap
    int count = 0;
    QMap<QString,addressHost>::const_iterator iterServer = m_serverMap.constBegin();
    while (iterServer != m_serverMap.constEnd()) {
        QString ip = iterServer.key();
        if(iterServer.value().isConnect){
            udpSocket->writeDatagram(sendstr.toUtf8(),QHostAddress(ip),my_port);
        }

        m_serverMap[ip].errCount++;
        if(m_serverMap[ip].errCount > 5 && m_serverMap[ip].errCount < 10){
            //连接断开了
            m_serverMap[ip].isConnect = false;
        }else if(m_serverMap[ip].errCount > 10)
        {
            //重新连接
            concet_send(m_serverMap[ip].ip,tcp_port);
            m_serverMap[ip].errCount = 5;
        }

        //列表显示
        m_serverModel->setItem(count,0,new QStandardItem(ip));
        m_serverModel->setItem(count,1,new QStandardItem(QString::number(m_serverMap[ip].port)));
        if(m_serverMap[ip].isConnect){
            m_serverModel->setItem(count,2,new QStandardItem("正常"));
        }else{
            m_serverModel->setItem(count,2,new QStandardItem("错误"));
        }
        ui->serverTableView->update();

        iterServer++;
        count++;
    }

    //检查是否有连接断开m_clientMap
    count = 0;
    QMap<QString,addressHost>::const_iterator iterClient = m_clientMap.constBegin();
    while (iterClient != m_clientMap.constEnd()) {
        QString ip = iterClient.key();
        m_clientMap[ip].errCount++;
//        qDebug()<<"errCount:"<<m_clientMap[ip].errCount<<",ip:"<<ip;
        if(m_clientMap[ip].errCount > 5){
            //连接断开了
            m_clientMap[ip].isConnect = false;
        }else if(m_clientMap[ip].errCount > 60000)
        {
            m_clientMap[ip].errCount = 10000;
        }

        //列表显示
        m_clientModel->setItem(count,0,new QStandardItem(ip));
        m_clientModel->setItem(count,1,new QStandardItem(QString::number(m_clientMap[ip].port)));
        if(m_clientMap[ip].isConnect)
        {
            m_clientModel->setItem(count,2,new QStandardItem("正常"));
        }else{
            m_clientModel->setItem(count,2,new QStandardItem("错误"));
        }

        iterClient++;
        count++;
    }

}

//心跳消息处理-->发送心跳回应
void MainWindow::heartbeat_respond(QString ip)
{
    QString localIp = ui->localIPcomboBox->currentText();
    QString sendstr = QString("ckmshare;heartbeat2;%1").arg(localIp);
    if(m_clientMap.contains(ip))
    {
        udpSocket->writeDatagram(sendstr.toUtf8(),QHostAddress(ip),my_port);
        //心跳正常
        m_clientMap[ip].errCount = 0;
        m_clientMap[ip].isConnect = true;
//        qDebug()<<"heartbeat_respond";
    }
}

//心跳回应消息处理
void MainWindow::heartbeat2_respond(QString ip)
{
    if(m_serverMap.contains(ip))
    {
        //心跳正常
        m_serverMap[ip].errCount = 0;
        m_serverMap[ip].isConnect = true;
//        qDebug()<<"heartbeat2_respond";

    }
}

//获取粘贴板内容
void MainWindow::get_clipboard_data()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if(mimeData->hasImage()){
        //图片
        qDebug()<<"图片";
        m_clipboardType = 2;
        m_clipImg = qvariant_cast<QPixmap>(mimeData->imageData());
    }else if(mimeData->hasHtml()){
        qDebug()<<"html";
        m_clipboardType = 1;
        m_clipData = mimeData->text();
//        qDebug()<<m_clipData;
    }else if(mimeData->hasText()){
        qDebug()<<"text";
        m_clipboardType = 1;
        m_clipData = mimeData->text();
//        qDebug()<<m_clipData;
    }else if(mimeData->hasUrls()){
        qDebug()<<"url";
        m_clipboardType = 1;
        m_clipData = mimeData->text();
//        qDebug()<<m_clipData;
    }
}

//发送粘贴板内容
void MainWindow::send_clipboard_data(QString ip)
{
    if(m_clipboardType == 1){
        //文字
        QString localIp = ui->localIPcomboBox->currentText();
        QString sendstr = QString("ckmshare;text;%1;%2").arg(localIp).arg(m_clipData);
        udpSocket->writeDatagram(sendstr.toUtf8(),QHostAddress(ip),my_port);
    }else if(m_clipboardType == 2){
        //图片
        QString localIp = ui->localIPcomboBox->currentText();
        QString sendstr = QString("ckmshare;image;%1").arg(localIp);
        fileData = get_imagedata_from_imagefile(m_clipImg.toImage());
        udpSocket->writeDatagram(sendstr.toUtf8(),QHostAddress(ip),my_port);
        //tcp
        int port = -1;
        if(m_clientMap.contains(ip)){
            port = m_clientMap.value(ip).port;
//            if(!m_clientMap.value(ip).isConnect){
//                //连接状态错误不发送
//                return;
//            }
        }else if(m_serverMap.contains(ip)){
            port = m_serverMap.value(ip).port;
//            if(!m_serverMap.value(ip).isConnect){
//                return;
//            }
        }else{
            return;
        }
        tcpClient->connectServer(ip,port);
        tcpClient->sendData(fileData,m_clipboardType);
    }else if(m_clipboardType == 3){
        //文件
    }else{

    }
}

//设置粘贴板内容 文本
void MainWindow::set_clipboard_data(QString text)
{
    QClipboard *clipboard = QApplication::clipboard();
    isLocalChangeClip = true;
    clipboard->setText(text);
    qDebug()<<"设置粘贴板:"<<text;
}


void MainWindow::clipboardChanged()
{
    if(isLocalChangeClip){
        isLocalChangeClip = false;
    }else{
        qint64 tempsystime1 = QDateTime::currentMSecsSinceEpoch();
        qDebug()<<"粘贴板内容改变:"<<tempsystime1;
        if(std::abs(tempsystime1-m_tempsystime)<200)
        {
            qDebug()<<"粘贴板内容改变<200,return";
            return;
        }
        else{
            m_tempsystime = tempsystime1;
        }
        get_clipboard_data();
        QMap<QString,addressHost>::const_iterator iterServer = m_serverMap.constBegin();
        while (iterServer != m_serverMap.constEnd()) {
            QString ip = iterServer.key();
            if(iterServer.value().isConnect){
                send_clipboard_data(ip);
            }
            iterServer++;
        }

        QMap<QString,addressHost>::const_iterator iterClient = m_clientMap.constBegin();
        while (iterClient != m_clientMap.constEnd()) {
            QString ip = iterClient.key();
            if(iterClient.value().isConnect){
                send_clipboard_data(ip);
            }
            iterClient++;
        }

    }

}


void MainWindow::tcpClientResult()
{
    qDebug()<<"tcpClientResult()";
    tcpClient->closeClient();
    fileData.clear();
}
void MainWindow::tcpClientSendProgress(int sendSize,int fileSize)
{
    qDebug()<<"数据发送:"<<(float(sendSize)/fileSize*100)<<"%";
}
void MainWindow::tcpServerResult(QByteArray data,int type)
{
    if(type == 2)
    {
        //图片
        QClipboard *clipboard = QApplication::clipboard();
        isLocalChangeClip = true;
        QImage img = get_imagedata_from_byte(data);
        m_clipImg = QPixmap::fromImage(img);
        clipboard->setImage(img);
    }else if(type == 3)
    {
        //文件
    }
//    tcpServer->closeServer();
}
void MainWindow::tcpServerProgress(int ReceivSzie,int fileSize)
{
    qDebug()<<"数据接收:"<<float(ReceivSzie)/fileSize<<"%";
}

QImage MainWindow::get_imagedata_from_byte(const QString &data)
{
    QByteArray imageData = QByteArray::fromBase64(data.toLatin1());
    QImage image;
    image.loadFromData(imageData);
    return image;
}

QByteArray MainWindow::get_imagedata_from_imagefile(const QImage &image)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    image.save(&buffer, "jpg");
    imageData = imageData.toBase64();
    return imageData;
}

void MainWindow::on_localIPcomboBox_currentIndexChanged(const QString &arg1)
{
    //开始监听tcp或者文件传输
//    tcpServer->closeServer();
    QString localIp = arg1;//ui->localIPcomboBox->currentText();
//    tcpServer->startListen(localIp,tcp_port);
    tcpServer->closeServer();
    tcpServer->startListen(localIp,tcp_port);
}

void MainWindow::on_actionslkdj_triggered()
{
    DialogTestTCP dlg;
    dlg.exec();
}

void MainWindow::on_actionserver_triggered()
{
    DialogTcpTest dlg;
    dlg.exec();
}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        //单击后显示主程序窗口
        this->show();
        m_sysTrayIcon->hide();
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    //最小化到系统托盘
    ExitDialog dlg;
    dlg.exec();
    e->ignore();  //忽略退出信号，程序继续运行
    if(dlg.get_is_ok())
    {
        if(dlg.get_is_exit())
        {
            e->accept();    //接受退出信号，程序退出
        }else{
            this->hide();
            setTaryIcon();
        }
    }

}

void MainWindow::setTaryIcon()
{
    //新建托盘要显示的icon
    QIcon icon = QIcon(":/new/prefix1/icon/ajwzr-67kl1-001.ico");
    //将icon设到QSystemTrayIcon对象中
    m_sysTrayIcon->setIcon(icon);
    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    m_sysTrayIcon->setToolTip(QObject::trUtf8("cmkShare"));
    //在系统托盘显示此对象
    m_sysTrayIcon->show();
}

void MainWindow::updataSystemIni()
{
    QSettings config("./SYSTEM.INI",QSettings::IniFormat);
    tcp_port = config.value("/set/port").toInt();
    m_fileSize = config.value("/set/fileSize").toInt();
    m_tempFilePath = config.value("/set/TempFilePath").toString();

}
