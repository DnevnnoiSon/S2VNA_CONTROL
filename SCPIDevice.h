#ifndef SCPIDEVICE_H
#define SCPIDEVICE_H

#include <QString>
#include <QVariant>
#include <QVariantMap>

class SCPIDevice {
public:
    virtual ~SCPIDevice() = default;

    // Генерация 1 команды по имени операции и параметрам
    virtual QString generateCommand(const QString& operation, const QVariantMap& config) = 0;

    // Парсинг ответа устройства: возвр. параметр -
    virtual QVariant parseResponse(const QString& response) = 0;

    // Преобразование числа в научную нотацию
    static QString toScientific(double value, int precision = 6) {
        return QString::number(value, 'e', precision);
    }
};

#endif // SCPIDEVICE_H
