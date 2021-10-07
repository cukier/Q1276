#ifndef RF1276DATA_H
#define RF1276DATA_H

#include <QObject>

class RF1276Data : public QObject
{
    Q_OBJECT

public:
    typedef enum baud_rate_enum {
        B1200BPS = 1,
        B2400BPS,
        B4800BPS,
        B9600BPS,
        B19200BPS,
        B38400BPS,
        B57600BPS,
        BINVPS
    } baud_rate_t;

    typedef enum parity_enum {
        NO_PARITY, ODD_PARITY, EVEN_PARITY
    } parity_t;

    typedef enum rf_factor_enum {
        RF_128 = 7, RF_256, RF_512, RF_1024, RF_2048, RF_4096
    } rf_factor_t;

    typedef enum mode_enum {
        MODE_STANDARD, MODE_LOW_POWER, MODE_SLEEP
    } radio_mode_t;

    typedef enum rf_bw_enum {
        BW_62_5K = 6, BW_125K, BW_250K, BW_500K
    } rf_bw_t;

    typedef enum rf_power_enum {
        P_4DBM = 1, P_7DBM, P_10DBM, P_13DBM, P_14DBM, P_17DBM, P_20DBM
    } rf_power_t;

    explicit RF1276Data(QObject *parent = nullptr);

    int parse(QByteArray &);
    QString toString(void) const;
    void clear();

    static const int RF1276_DATA_SIZE = 12;

    baud_rate_t baudrate;
    parity_t parity;
    float frequency;
    rf_factor_t rf_factor;
    radio_mode_t mode;
    rf_bw_t rf_bw;
    uint16_t id;
    uint8_t net_id;
    rf_power_t rf_power;
};

#endif // RF1276DATA_H
