#ifndef TRANSIMAGE_H
#define TRANSIMAGE_H

#include <QObject>
#include <QTcpSocket>
#include <QImage>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>

//用于传输图片

class transImage : public QObject
{
    Q_OBJECT
public:
    explicit transImage(QObject *parent = nullptr);
    QByteArray pixmapToByte(const QPixmap &img);       //QPixmap-->QByteArray
    QPixmap ByteToPixmap(const QString &data);             //QByteArray-->QPixmap

public slots:
    void sendImageH(QPixmap img,QString ip,int port);       //传输图片

signals:

};

#endif // TRANSIMAGE_H
