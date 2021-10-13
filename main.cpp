#include <QCoreApplication>
#include <iostream>
#include <string>

#include "rf1276.h"
#include "serialport.h"

//porta serial
#define PORT_ARG            "-p"
#define BAUD_ARG            "-b"
//comando
#define READ_CMD            "-r"
#define WRITE_CMD           "-w"
//parametros comando escrita
#define SET_BAUD_CMD        "-sb"
#define SET_PARITY_CMD      "-sp"
#define SET_FREQ_CMD        "-sf"
#define SET_RF_FA_CMD       "-sfa"
#define SET_MODE_CMD        "-sm"
#define SET_RF_BW_CMD       "-sbw"
#define SET_ID_CMD          "-si"
#define SET_NET_ID_CMD      "-sn"
#define SET_RF_PW_CMD       "-spw"
//parametros paridade
#define SET_PARITY_NONE     "none"
#define SET_PARITY_ODD      "odd"
#define SET_PARITY_EVEN     "even"
//parametros modo de operacao
#define SET_STD_MODE        "standard"
#define SET_LOW_MODE        "low-power"
#define SET_SLP_MODE        "sleep"
//parametros rf bw
#define SET_62_5K           "62.5k"
#define SET_125K            "125k"
#define SET_256K            "256k"
#define SET_512K            "512k"
//parametros rf power
#define SET_4DBM            "4dBm"
#define SET_7DBM            "7dBm"
#define SET_10DBM           "10dBm"
#define SET_13DBM           "13dBm"
#define SET_14DBM           "14dBm"
#define SET_17DBM           "17dBm"
#define SET_20DBM           "20dBm"

int main(int argc, char *argv[])
{
    if (argc > 1) {
        QCoreApplication *a = nullptr;
        RF1276 *rf = nullptr;
        int indexPorta = -1;
        int indexComandoWr = -1;
        int indexComandoRd = -1;
        int indexBaud = -1;
        int indexBaudValue = -1;
        int indexParityValue = -1;
        int indexFreqValue = -1;
        int indexFRfFactValue = -1;
        int indexModeValue = -1;
        int indexRfBwValue = -1;
        int indexIdValue = -1;
        int indexNetIdValue = -1;
        int indexRfPowValue = -1;


        for (int i = 0; i < argc; ++i) {
            std::string str(argv[i]);

            if (!str.compare(PORT_ARG)) {
                indexPorta = i + 1;
            } else if (!str.compare(BAUD_ARG)) {
                indexBaud = i + 1;
            } else if (!str.compare(READ_CMD)) {
                indexComandoRd = i;
            } else if (!str.compare(WRITE_CMD)) {
                indexComandoWr = i;
            } else if (!str.compare(SET_BAUD_CMD)) {
                indexBaudValue = i + 1;
            } else if (!str.compare(SET_PARITY_CMD)) {
                indexParityValue = i + 1;
            } else if (!str.compare(SET_FREQ_CMD)) {
                indexFreqValue = i + 1;
            } else if (!str.compare(SET_RF_FA_CMD)) {
                indexFRfFactValue = i + 1;
            } else if (!str.compare(SET_MODE_CMD)) {
                indexModeValue = i + 1;
            } else if (!str.compare(SET_RF_BW_CMD)) {
                indexRfBwValue = i + 1;
            } else if (!str.compare(SET_ID_CMD)) {
                indexIdValue = i + 1;
            } else if (!str.compare(SET_NET_ID_CMD)) {
                indexNetIdValue = i + 1;
            } else if (!str.compare(SET_RF_PW_CMD)) {
                indexRfPowValue = i + 1;
            }
        }

        if (indexPorta == -1) {
            std::cout << "Necessario indicar porta serial '-p porta'" << std::endl;
            return -1;
        }

        if (indexBaud == -1) {
            std::cout << "Necessario indicar baud para porta serial '-b baud'" << std::endl;
            return -1;
        }

        if ((indexComandoWr == -1) && (indexComandoRd == -1)) {
            std::cout << "Necessario indicar um comando escrita '-w' ou leitura '-r'\n";
            return -1;
        }

        QSerialPort::BaudRate baud = QSerialPort::UnknownBaud;

        try {
            int baudInt = std::stoi(argv[indexBaud]);

            if (baudInt == 9600) {
                baud = QSerialPort::Baud9600;
            } else if (baudInt == 19200) {
                baud = QSerialPort::Baud19200;
            } else {
                std::cout << "Baud invalido usar 9600 ou 19200\n";
                return -1;
            }
        }  catch (std::invalid_argument const&) {
            std::cout << "Baud invalido usar 9600 ou 19200\n";
            return -1;
        }

        if (indexComandoRd != -1) {
            a = new QCoreApplication(argc, argv);
            rf = new RF1276(a, argv[indexPorta], baud);
            rf->readRadio();
        } else if (indexComandoWr != -1) {
            RF1276Data::baud_rate_t newBaud = RF1276Data::BINVPS;
            RF1276Data::parity_t newParity = RF1276Data::INV_PARITY;
            float newFreq = -1.0f;
            RF1276Data::rf_factor_t newRfFactor = RF1276Data::RF_INV;
            RF1276Data::radio_mode_t newMode = RF1276Data::MODE_INV;
            RF1276Data::rf_bw_t newRfBw = RF1276Data::BW_INV;
            int newId = -1;
            int newNetId = -1;
            RF1276Data::rf_power_t newPw = RF1276Data::P_INV;

            if (indexBaudValue != -1) {
                try {
                    int newBaudCmd = std::stoi(argv[indexBaudValue]);

                    if ((newBaudCmd != 9600) && (newBaudCmd != 19200)) {
                        std::cout << "Baud invalido usar 9600 ou 19200\n";
                        return -1;
                    } else {
                        if (newBaudCmd == 19200)
                            newBaud = RF1276Data::B19200BPS;
                        else if(newBaudCmd == 9600)
                            newBaud = RF1276Data::B9600BPS;
                    }
                }  catch (std::invalid_argument const&) {
                    std::cout << "Baud invalido usar 9600 ou 19200\n";
                    return -1;
                }
            }

            if (indexParityValue != -1) {
                std::string str(argv[indexParityValue]);

                if (!str.compare(SET_PARITY_NONE)) {
                    newParity = RF1276Data::NO_PARITY;
                } else if (!str.compare(SET_PARITY_EVEN)) {
                    newParity = RF1276Data::EVEN_PARITY;
                } else if (!str.compare(SET_PARITY_ODD)) {
                    newParity = RF1276Data::ODD_PARITY;
                } else {
                    std::cout << "Paridade errada, uar "
                              << SET_PARITY_NONE
                              << " ou " << SET_PARITY_EVEN
                              << " ou " << SET_PARITY_ODD
                              << std::endl;
                }
            }

            if (indexFreqValue != -1) {
                try {
                    newFreq = std::stof(argv[indexFreqValue]);

                    if (newFreq < 915.0 || newFreq > 928.0) {
                        std::cout << "Frequencia invalida, por favor usar 915.0 a 928.0 (MHz)\n";
                        return -1;
                    }
                }  catch (std::invalid_argument const&) {
                    std::cout << "Frequencia invalida, por favor usar 915.0 a 928.0 (MHz)\n";
                    return -1;
                }
            }

            if (indexFRfFactValue != -1) {
                try {
                    int rfFactorAux = std::stoi(argv[indexFRfFactValue]);

                    if ((rfFactorAux != 128) && (rfFactorAux != 256) &&
                            (rfFactorAux != 512) && (rfFactorAux != 1024) &&
                            (rfFactorAux != 2048) && (rfFactorAux != 4096)) {
                        std::cout << "Rf Factor invalido, por favor usar 128, 256, 512, 1024, 2048 ou 4096\n";
                        return -1;
                    }


                    if (rfFactorAux == 128)
                        newRfFactor = RF1276Data::RF_128;
                    if (rfFactorAux == 256)
                        newRfFactor = RF1276Data::RF_256;
                    if (rfFactorAux == 512)
                        newRfFactor = RF1276Data::RF_512;
                    if (rfFactorAux == 1024)
                        newRfFactor = RF1276Data::RF_1024;
                    if (rfFactorAux == 2048)
                        newRfFactor = RF1276Data::RF_2048;
                    if (rfFactorAux == 4096)
                        newRfFactor = RF1276Data::RF_4096;

                }  catch (std::invalid_argument const&) {
                    std::cout << "Rf Factor invalido, por favor usar 128, 256, 512, 1024, 2048 ou 4096\n";
                    return -1;
                }
            }

            if (indexModeValue != -1) {
                std::string str = std::string(argv[indexModeValue]);

                if (!str.compare(SET_STD_MODE)) {
                    newMode = RF1276Data::MODE_STANDARD;
                } else if (!str.compare(SET_LOW_MODE)) {
                    newMode = RF1276Data::MODE_LOW_POWER;
                } else if (!str.compare(SET_SLP_MODE)) {
                    newMode = RF1276Data::MODE_SLEEP;
                } else {
                    std::cout << "Modo invalido, por favor usar"
                              << SET_STD_MODE << " ou "
                              << SET_LOW_MODE << " ou "
                              << SET_SLP_MODE << std::endl;
                    return -1;
                }
            }

            if (indexRfBwValue != -1) {
                std::string str = std::string(argv[indexRfBwValue]);

                if (!str.compare(SET_62_5K)) {
                    newRfBw = RF1276Data::BW_62_5K;
                } else if (!str.compare(SET_125K)) {
                    newRfBw = RF1276Data::BW_125K;
                } else if (!str.compare(SET_256K)) {
                    newRfBw = RF1276Data::BW_256K;
                } else if (!str.compare(SET_512K)) {
                    newRfBw = RF1276Data::BW_512K;
                } else {
                    std::cout << "Rf Bw invalido, por favor usar"
                              << SET_62_5K << " ou "
                              << SET_125K << " ou "
                              << SET_256K << " ou "
                              << SET_512K << std::endl;
                    return -1;
                }
            }

            if (indexIdValue != -1) {
                try {
                    int indexIdAux = std::stoi(argv[indexIdValue]);

                    if (indexIdAux < 0 || indexIdAux > 0xFFFF) {
                        std::cout << "ID invalido, por favor usar 0 ~ 65535\n";
                        return -1;
                    }

                    newId = indexIdAux;
                }  catch (std::invalid_argument const&) {
                    std::cout << "ID invalido, por favor usar 0 ~ 65535\n";
                    return -1;
                }
            }

            if (indexNetIdValue != -1) {
                try {
                    int indexNetIdAux = std::stoi(argv[indexIdValue]);

                    if (indexNetIdAux < 0 || indexNetIdAux > 0xFF) {
                        std::cout << "Net ID invalido, por favor usar 0 ~ 254\n";
                        return -1;
                    }

                    newNetId = indexNetIdAux;
                }  catch (std::invalid_argument const&) {
                    std::cout << "Net ID invalido, por favor usar 0 ~ 254\n";
                    return -1;
                }
            }

            if (indexRfPowValue != -1) {
                std::string str = std::string(argv[indexRfBwValue]);

                if (!str.compare(SET_4DBM)) {
                    newPw = RF1276Data::P_4DBM;
                } else if (!str.compare(SET_7DBM)) {
                    newPw = RF1276Data::P_7DBM;
                } else if (!str.compare(SET_10DBM)) {
                    newPw = RF1276Data::P_10DBM;
                } else if (!str.compare(SET_13DBM)) {
                    newPw = RF1276Data::P_13DBM;
                } else if (!str.compare(SET_14DBM)) {
                    newPw = RF1276Data::P_14DBM;
                } else if (!str.compare(SET_17DBM)) {
                    newPw = RF1276Data::P_17DBM;
                } else if (!str.compare(SET_20DBM)) {
                    newPw = RF1276Data::P_20DBM;
                } else {
                    std::cout << "Rf Power invalido, por favor usar"
                              << SET_4DBM << " ou "
                              << SET_7DBM << " ou "
                              << SET_10DBM << " ou "
                              << SET_13DBM << " ou "
                              << SET_14DBM << " ou "
                              << SET_17DBM << " ou "
                              << SET_20DBM << std::endl;
                    return -1;
                }
            }


            a = new QCoreApplication(argc, argv);
            rf = new RF1276(a, argv[indexPorta], baud);

            if (newBaud != RF1276Data::BINVPS) {
                rf->setBaud(newBaud);
            }

            if (newParity != RF1276Data::INV_PARITY) {
                rf->setParity(newParity);
            }

            if (newFreq != -1.0) {
                rf->setFrequencie(newFreq * 1000000);
            }

            if (newRfFactor != RF1276Data::RF_INV) {
                rf->setRfFactor(newRfFactor);
            }

            if (newMode != RF1276Data::MODE_INV) {
                rf->setMode(newMode);
            }

            if (newRfBw != RF1276Data::BW_INV) {
                rf->setRfBW(newRfBw);
            }

            if (newId != -1) {
                rf->setId(newId);
            }

            if (newNetId != -1) {
                rf->setNetID(newNetId);
            }

            if (newPw != RF1276Data::P_INV) {
                rf->setRfPower(newPw);
            }

            rf->writeRadio();
        }

        if (a == nullptr)
            return -1;

        return a->exec();
    } else {
        std::cout << "falta de argumentos" << std::endl;
        return -1;
    }
}
