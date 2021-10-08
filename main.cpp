#include <QCoreApplication>
#include <iostream>
#include <string>

#include "rf1276.h"
#include "serialport.h"

#define PORT_ARG        "-p"
#define BAUD_ARG        "-b"
#define READ_CMD        "-r"
#define WRITE_CMD       "-w"

int main(int argc, char *argv[])
{
    if (argc > 1) {
        QCoreApplication a(argc, argv);
        RF1276 *rf = nullptr;
        int indexPorta = -1;
        int indexComando = -1;
        int indexBaud = -1;


        for (int i = 0; i < argc; ++i) {
            std::string str(argv[i]);

            if (!str.compare(PORT_ARG)) {
                indexPorta = i + 1;
            }

            if (!str.compare(BAUD_ARG)) {
                indexBaud = i + 1;
            }

            if (!str.compare(READ_CMD))
                indexComando = i;
            else if (!str.compare(WRITE_CMD))
                indexComando = i + 1;
        }

        if (indexPorta != -1) {
            if (indexBaud != -1) {
                std::string strBaud(argv[indexBaud]);
                int baudInt = std::stoi(strBaud);
                QSerialPort::BaudRate baud = QSerialPort::UnknownBaud;

                if (baudInt == 9600) {
                    baud = QSerialPort::Baud9600;
                } else if (baudInt == 19200) {
                    baud = QSerialPort::Baud19200;
                } else {
                    std::cout << "Baud nao valido" << std::endl;
                    return -1;
                }

                if (indexComando != -1) {
                    std::string str(argv[indexComando]);

                    if (!str.compare(READ_CMD)) {
                        rf = new RF1276(&a, argv[indexPorta], baud);
                        rf->readRadio();
                    }
                } else {
                    std::cout << "Necessario indicar um comando escrita '-w' ou leitura '-r'" << std::endl;
                    return -1;
                }
            } else {
                std::cout << "Necessario indicar baud para porta serial '-b baud'" << std::endl;
                return -1;
            }
        } else {
            std::cout << "Necessario indicar porta serial '-p porta'" << std::endl;
            return -1;
        }
        //        RF1276 rf(&a, QString::fromStdString(argv[1]));

        //        rf.readRadio();
        //        rf.setBaud(RF1276Data::B19200BPS);

        //    quin

        //    SerialPort p(&a, "/dev/ttyUSB1");
        //    QByteArray request;

        //    request.append(0x01);
        //    request.append(0x03);
        //    request.append((quint8) 0x00);
        //    request.append((quint8) 0x00);
        //    request.append((quint8) 0x00);
        //    request.append(0x0a);
        //    request.append(0xc5);
        //    request.append(0xcd);

        //    p.write(request);
        //    p.close();

        return a.exec();
    } else {
        std::cout << "falta de argumentos" << std::endl;
        return -1;
    }
}
