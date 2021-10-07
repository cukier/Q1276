#ifndef RF1276_H
#define RF1276_H

#include <QObject>

#include "rf1276data.h"

class SerialPort;


class RF1276 : public QObject
{
    Q_OBJECT

public:
    typedef enum command_yy_enum {
        CMD_WRITE = 1, CMD_READ, CMD_STANDARD, CMD_CENTRAL, CMD_NODE, CMD_RSSI
    } command_yy_t;

    typedef enum command_xx_enum {
        CMD_XX_RESPONSE = 0x00, CMD_XX_SENDING = 0x80
    } command_xx_t;

    explicit RF1276(QObject *parent = nullptr, QString portName = 0);
    void readRadio() const;

signals:

private slots:
    void onRequest(QByteArray &) const;

private:
    quint8 crc(const QByteArray &) const;
    QByteArray makeRequest(const command_yy_t, const QByteArray &, const quint16) const;
    QByteArray makeReadCommand() const;

    SerialPort *port;

};

#endif // RF1276_H
