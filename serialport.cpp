#include "serialport.h"

#include <QDebug>
#include <QCoreApplication>

#define serialTimeOut         100

SerialPort::SerialPort(QObject *parent,
                       QString portname,
                       QSerialPort::BaudRate baud) :
    QObject(parent),
    m_serialPort(new QSerialPort(this)),
    m_portname(portname)
{
    m_serialPort->setPortName(m_portname);
    m_serialPort->setBaudRate(baud);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);

    connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialPort::handleError);
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialPort::handleReadyRead);
    connect(&m_timer, &QTimer::timeout, this, &SerialPort::handleTimeout);
}

SerialPort::~SerialPort()
{
    if (m_serialPort->isOpen())
        m_serialPort->close();

    delete m_serialPort;
}

void SerialPort::write(const QByteArray &writeData)
{
    openPort();

    if (m_buffer.size())
        m_buffer.clear();

    const qint64 bytesWritten = m_serialPort->write(writeData);

    if (bytesWritten == -1) {
        qDebug() << tr("Failed to write the data to port %1, error: %2")
                    .arg(m_serialPort->portName(), m_serialPort->errorString());
    } else if (bytesWritten != writeData.size()) {
        qDebug() << tr("Failed to write all the data to port %1, error: %2")
                    .arg(m_serialPort->portName(), m_serialPort->errorString());
        //    } else if (bytesWritten == writeData.size()) {
        //        qDebug() << "write ok";
    }

    m_timer.start(serialTimeOut);
}

void SerialPort::close()
{
    if (m_serialPort->isOpen())
        m_serialPort->close();
}

void SerialPort::openPort()
{
    bool open = false;

    if(!m_serialPort->isOpen())
        open = m_serialPort->open(QIODevice::ReadWrite);

    qDebug().noquote() << QString("Porta %1@%2 : %3").arg(m_portname).arg(m_serialPort->baudRate()).arg(open);
}

void SerialPort::handleReadyRead()
{
    m_buffer.append(m_serialPort->readAll());
    m_timer.start(serialTimeOut);
}

void SerialPort::handleTimeout()
{
    close();
    m_timer.stop();
    emit newResponse(m_buffer);
}

void SerialPort::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError) {
        qDebug() << tr("An I/O error occurred while reading "
                       "the data from port %1, error: %2")
                    .arg(m_serialPort->portName(), m_serialPort->errorString());
    }
}
