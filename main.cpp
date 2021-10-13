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
        //        int indexFreqValue = -1;
        //        int indexFRfFactValue = -1;
        //        int indexModeValue = -1;
        //        int indexRfBwValue = -1;
        //        int indexIdValue = -1;
        //        int indexNetIdValue = -1;
        //        int indexRfPowValue = -1;


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
                //            } else if (!str.compare(SET_FREQ_CMD)) {
                //                indexFreqValue = i + 1;
                //            } else if (!str.compare(SET_RF_FA_CMD)) {
                //                indexFRfFactValue = i + 1;
                //            } else if (!str.compare(SET_MODE_CMD)) {
                //                indexModeValue = i + 1;
                //            } else if (!str.compare(SET_RF_BW_CMD)) {
                //                indexRfBwValue = i + 1;
                //            } else if (!str.compare(SET_ID_CMD)) {
                //                indexIdValue = i + 1;
                //            } else if (!str.compare(SET_NET_ID_CMD)) {
                //                indexNetIdValue = i + 1;
                //            } else if (!str.compare(SET_RF_PW_CMD)) {
                //                indexRfPowValue = i + 1;
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

            a = new QCoreApplication(argc, argv);
            rf = new RF1276(a, argv[indexPorta], baud);

            if (newBaud != RF1276Data::BINVPS) {
                rf->setBaud(newBaud);
            }

            if (newParity != RF1276Data::INV_PARITY) {
                rf->setParity(newParity);
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
