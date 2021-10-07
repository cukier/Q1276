#include "rf1276.h"
#include "serialport.h"

#include <QDebug>
#include <QCoreApplication>

RF1276::RF1276(QObject *parent, QString portName) :
    QObject(parent),
    port(new SerialPort(this, portName))
{
    connect(port, &SerialPort::newResponse, this, &RF1276::onRequest);
}

quint8 RF1276::crc(const QByteArray &data) const
{
    quint16 acm = 0;

    for (quint8 i : data) {
        acm += i;
    }

    return acm;
}

QByteArray RF1276::makeRequest(const command_yy_t cmd_type, const QByteArray &data, quint16 size) const
{
    QByteArray request;

    request.append(0xAF);
    request.append(0xAF);
    request.append((quint8) 0x00);
    request.append((quint8) 0x00);
    request.append(0xAF);
    request.append((quint8) CMD_XX_SENDING);
    request.append((quint8) cmd_type);
    request.append((quint8) size);

    int cont;

    for (cont = 0; cont < size; ++cont)
        request.append(data[cont]);

    request.append(crc(request));
    request.append(0x0D);
    request.append(0x0A);

    return request;
}

QByteArray RF1276::makeReadCommand() const
{
    return makeRequest(CMD_READ,
                       QByteArray(RF1276Data::RF1276_DATA_SIZE, (char) 0),
                       RF1276Data::RF1276_DATA_SIZE);
}

void RF1276::readRadio() const
{
    QByteArray readRequest = makeReadCommand();

    port->write(readRequest);
}

void RF1276::onRequest(QByteArray &request) const
{
    QByteArray data;

    for(int i = 0; i < 12; ++i)
        data.append(request.at(8 + i));

    RF1276Data radio;

    radio.parse(data);
    QString radioStr = radio.toString();
    qDebug().noquote() << radioStr.toUtf8();
    QCoreApplication::exit(0);
}
