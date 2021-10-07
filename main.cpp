#include <QCoreApplication>
#include <iostream>

#include "rf1276.h"
//#include "serialport.h"

int main(int argc, char *argv[])
{
    if (argc > 1) {
        QCoreApplication a(argc, argv);
        RF1276 rf(&a, QString::fromStdString(argv[1]));

        rf.readRadio();

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
