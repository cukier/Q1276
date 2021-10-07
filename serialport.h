#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr, QString portname = 0);
    ~SerialPort();

    void write(const QByteArray &writeData);

signals:
    void newResponse(QByteArray &response);

private slots:
    void openPort();
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError);

private:
    QSerialPort *m_serialPort = nullptr;
    QTimer m_timer;
    QByteArray m_buffer;
    QString m_portname;

    void close();
};

#endif // SERIALPORT_H
