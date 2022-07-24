#include "transimage.h"

transImage::transImage(QObject *parent) : QObject(parent)
{

}

void transImage::sendImageH(QPixmap img,QString ip,int port)
{
}

QByteArray transImage::pixmapToByte(const QPixmap &img)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    img.toImage().save(&buffer,"PNG");
    return imageData;
}

QPixmap transImage::ByteToPixmap(const QString &data)
{
}
