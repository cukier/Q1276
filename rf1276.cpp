#include "rf1276.h"
#include "serialport.h"

#include <QDebug>
#include <QCoreApplication>

RF1276::RF1276(QObject *parent, QString portName, QSerialPort::BaudRate baud) :
    QObject(parent),
    port(new SerialPort(this, portName, baud)),
    ex(NoExec)
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

quint8 RF1276::toUchar(const quint32 &in, const quint32 &index) const
{
    quint32 mask = 0xFF << index * 8;
    quint32 aux = in & mask;

    aux >>= index * 8;
    aux &= 0xFF;

    return (quint8) aux;
}

QByteArray RF1276::freqToUchar(const float &freq_hz) const
{
    QByteArray ret;

    quint32 aux = (quint32) (freq_hz / 61.035f);

    ret.append(toUchar(aux, 0));
    ret.append(toUchar(aux, 1));
    ret.append(toUchar(aux, 2));

    return ret;
}


QByteArray RF1276::makeWriteCommand(const RF1276Data &data) const
{
    QByteArray request;

    request.append((quint8) data.baudrate);
    request.append((quint8) data.parity);

    QByteArray freq = freqToUchar(data.frequency);

    request.append((quint8) freq.at(2));
    request.append((quint8) freq.at(1));
    request.append((quint8) freq.at(0));
    request.append((quint8) data.rf_factor);
    request.append((quint8) data.mode);
    request.append((quint8) data.rf_bw);
    request.append((quint8) ((data.id & 0xFF00) >> 8) & 0xFF);
    request.append((quint8) (data.id & 0xFF));
    request.append((quint8) data.net_id);
    request.append((quint8) data.rf_power);

    return makeRequest(CMD_WRITE,
                       request,
                       RF1276Data::RF1276_DATA_SIZE);
}

void RF1276::onRequest(QByteArray &request)
{
    if (request.length()) {
        QByteArray data;

        for(int i = 0; i < 12; ++i)
            data.append(request.at(8 + i));

        RF1276Data radio;

        radio.parse(data);

        if (ex == ReadRadio) {
            QString radioStr = radio.toString();
            qDebug().noquote() << radioStr.toUtf8();
            QCoreApplication::exit(0);
        } else if (ex == WriteBaud) {
            radio.baudrate = radio_data.baudrate;
        } else if (ex == WriteParity) {
            radio.parity = radio_data.parity;
        } else if (ex == WriteFreq) {
            radio.frequency = radio_data.frequency;
        } else if (ex == WriteRfFactor) {
            radio.rf_factor = radio_data.rf_factor;
        } else if (ex == WriteRadioMode) {
            radio.mode = radio_data.mode;
        } else if (ex == WriteRfBW) {
            radio.rf_bw = radio_data.rf_bw;
        } else if (ex == WriteID) {
            radio.id = radio_data.id;
        } else if (ex == WriteNetID) {
            radio.net_id = radio_data.net_id;
        } else if (ex == WritePower) {
            radio.rf_power = radio_data.rf_power;
        }

        port->write(makeWriteCommand(radio));
    } else {
        qDebug() << "Sem resposta do radio ou erro de comunicacao";
        QCoreApplication::exit(0);
    }

    ex = NoExec;
}


void RF1276::readRadio()
{
    QByteArray readRequest = makeReadCommand();

    ex = ReadRadio;
    port->write(readRequest);
}

void RF1276::setBaud(const RF1276Data::baud_rate_t &baud)
{
    QByteArray readRequest = makeReadCommand();

    radio_data.clear();
    radio_data.baudrate = baud;
    ex = WriteBaud;
    port->write(readRequest);
}

void RF1276::setParity(const RF1276Data::parity_t &parity)
{
    QByteArray readRequest = makeReadCommand();

    radio_data.clear();
    radio_data.parity = parity;
    ex = WriteParity;
    port->write(readRequest);
}

void RF1276::setFrequencie(const float &frequencie)
{
    QByteArray readRequest = makeReadCommand();

    radio_data.clear();
    radio_data.frequency = frequencie;
    ex = WriteFreq;
    port->write(readRequest);
}

void RF1276::setRfFactor(const RF1276Data::rf_factor_t &factor)
{
    QByteArray readRequest = makeReadCommand();

    radio_data.clear();
    radio_data.rf_factor = factor;
    ex = WriteRfFactor;
    port->write(readRequest);
}

void RF1276::setMode(const RF1276Data::radio_mode_t &mode)
{
    QByteArray readRequest = makeReadCommand();

    radio_data.clear();
    radio_data.mode = mode;
    ex = WriteRadioMode;
    port->write(readRequest);
}

void RF1276::setRfBW(const RF1276Data::rf_bw_t &rf_bw)
{
    QByteArray readRequest = makeReadCommand();

    radio_data.clear();
    radio_data.rf_bw = rf_bw;
    ex = WriteRadioMode;
    port->write(readRequest);
}

void RF1276::setId(const quint16 &id)
{
    QByteArray readRequest = makeReadCommand();

    radio_data.clear();
    radio_data.id = id;
    ex = WriteID;
    port->write(readRequest);
}

void RF1276::setNetID(const quint8 &net_id)
{
    QByteArray readRequest = makeReadCommand();

    radio_data.clear();
    radio_data.net_id = net_id;
    ex = WriteNetID;
    port->write(readRequest);
}

void RF1276::setRfPower(const RF1276Data::rf_power_t &power)
{
    QByteArray readRequest = makeReadCommand();

    radio_data.clear();
    radio_data.rf_power = power;
    ex = WritePower;
    port->write(readRequest);
}
