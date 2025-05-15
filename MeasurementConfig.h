#ifndef MEASUREMENTCONFIG_H
#define MEASUREMENTCONFIG_H

#include <QString>

//====== КОНФИГУРАЦИЯ ВАЦ =======//
struct MeasurementConfig{
    double start_freq;
    double stop_freq;
    double step;
    double power;
    int samples;
};

struct Network{
    QString ip_addr;// для socket соединения
    int port;
};

//=========== Настройки передачи команд ===========//
struct Settings{
//все существующие типы соединения:
    Network network;
//О
};




#endif // MEASUREMENTCONFIG_H
