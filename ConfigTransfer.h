#ifndef CONFIGTRANSFER_H
#define CONFIGTRANSFER_H

#include <QString>

//====== КОНФИГУРАЦИЯ ВАЦ =======//
struct MeasurementConfig{
    double start_freq;
    double stop_freq;
    double step;
    double power;
    int samples;
};

// для socket соединения:
struct Network{
    QString ip_addr;
    int port;
};
//=========== Настройки передачи команд ===========//
struct Settings{
//все существующие типы соединения:
    Network network;
};

#endif // CONFIGTRANSFER_H
