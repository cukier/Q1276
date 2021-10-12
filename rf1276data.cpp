#include "rf1276data.h"

RF1276Data::RF1276Data(QObject *parent) : QObject(parent)
{

}

int RF1276Data::parse(QByteArray &data)
{
    if (data.length() == RF1276_DATA_SIZE){
        baudrate = (baud_rate_t) data.at(0);
        parity = (parity_t) data.at(1);

        frequency = float(((data.at(2) << 16) & 0xFF0000) |
                          ((data.at(3) << 8) & 0xFF00) |
                          (data.at(4) & 0xFF)) * 61.035f;

        rf_factor = (rf_factor_t) data.at(5);
        mode = (radio_mode_t) data.at(6);
        rf_bw = (rf_bw_t) data.at(7);
        id = (uint16_t) ((data.at(8) << 8) | (data.at(9)));
        net_id = (uint8_t) data.at(10);
        rf_power = (rf_power_t) data.at(11);

        return 0;
    }

    return -1;
}

QString RF1276Data::toString() const
{
    QString str;

    str.append("Baud:\t\t");

    if (baudrate == RF1276Data::B1200BPS)
        str.append("1200BPS");
    else if (baudrate == RF1276Data::B2400BPS)
        str.append("2400BPS");
    else if (baudrate == RF1276Data::B4800BPS)
        str.append("4800BPS");
    else if (baudrate == RF1276Data::B9600BPS)
        str.append("9600BPS");
    else if (baudrate == RF1276Data::B19200BPS)
        str.append("19200BPS");
    else if (baudrate == RF1276Data::B38400BPS)
        str.append("38400BPS");
    else if (baudrate == RF1276Data::B57600BPS)
        str.append("57600BPS");

    str.append("\nParity:\t\t");

    if (parity == RF1276Data::NO_PARITY)
        str.append("No Parity ");
    else if (parity == RF1276Data::EVEN_PARITY)
        str.append("Even Parity ");
    else if (parity == RF1276Data::ODD_PARITY)
        str.append("Odd Parity ");

    str.append(QString("\nFrequencie:\t%1 MHz").arg(frequency / 1000000.0));
    str.append("\nRF_Factor:\t");

    if (rf_factor == RF_128)
        str.append("128");
    else if (rf_factor == RF_256)
        str.append("256");
    else if (rf_factor == RF_512)
        str.append("512");
    else if (rf_factor == RF_1024)
        str.append("1024");
    else if (rf_factor == RF_2048)
        str.append("2048");
    else if (rf_factor == RF_4096)
        str.append("4096");

    str.append("\nMode:\t\t");

    if (mode == MODE_STANDARD)
        str.append("Standard");
    else if (mode == MODE_LOW_POWER)
        str.append("Low Power");
    else if (mode == MODE_SLEEP)
        str.append("Sleep");

    str.append("\nRF BW:\t\t");

    if (rf_bw == BW_62_5K)
        str.append("62.5K");
    else if (rf_bw == BW_125K)
        str.append("125K");
    else if (rf_bw == BW_250K)
        str.append("250K");
    else if (rf_bw == BW_500K)
        str.append("500K");

    str.append(QString("\nID:\t\t%1").arg(id));
    str.append(QString("\nNet ID:\t\t%1").arg(net_id));
    str.append("\nRF Power:\t");

    if (rf_power == P_4DBM)
        str.append("4dBm");
    else if (rf_power == P_7DBM)
        str.append("7dBm");
    else if (rf_power == P_10DBM)
        str.append("10dBm");
    else if (rf_power == P_13DBM)
        str.append("13dBm");
    else if (rf_power == P_14DBM)
        str.append("14dBm");
    else if (rf_power == P_17DBM)
        str.append("17dBm");
    else if (rf_power == P_20DBM)
        str.append("20dBm");

    return str;
}

void RF1276Data::clear()
{
    baudrate = BINVPS;
    parity = INV_PARITY;
    frequency = -1.0f;
    rf_factor = RF_INV;
    mode = MODE_INV;
    rf_bw = BW_INV;
    id = 0xFFFF;
    net_id = 0xFF;
    rf_power = P_4DBM;
}
