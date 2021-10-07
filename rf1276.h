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
    void readRadio();
    void setBaud(const RF1276Data::baud_rate_t&);
    void setParity(const RF1276Data::parity_t&);
    void setFrequencie(const float&);
    void setRfFactor(const RF1276Data::rf_factor_t&);
    void setMode(const RF1276Data::radio_mode_t&);
    void setRfBW(const RF1276Data::rf_bw_t&);
    void setId(const quint16&);
    void setNetID(const quint8&);
    void setRfPower(const RF1276Data::rf_power_t&);

signals:

private slots:
    void onRequest(QByteArray &);

private:
    typedef enum exec_enum {
        NoExec,
        ReadRadio,
        WriteBaud,
        WriteParity,
        WriteFreq,
        WriteRfFactor,
        WriteRadioMode,
        WriteRfBW,
        WriteID,
        WriteNetID,
        WritePower
    } execute_t;

    quint8 crc(const QByteArray &) const;
    QByteArray makeRequest(const command_yy_t, const QByteArray &, const quint16) const;
    QByteArray makeReadCommand() const;
    quint8 toUchar(const quint32&, const quint32&) const;
    QByteArray freqToUchar(const float&) const;
    QByteArray makeWriteCommand(const RF1276Data&) const;

    SerialPort *port;
    execute_t ex;
    RF1276Data radio_data;
};

#endif // RF1276_H
