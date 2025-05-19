#ifndef S2VNADEVICE_H
#define S2VNADEVICE_H

#include"SCPIDevice.h"

class S2VNADevice : public SCPIDevice {
public:
    // Полное определение вложенного класса внутри S2VNADevice
    class CommandGenerator {
    public:
        explicit CommandGenerator(S2VNADevice* dev = nullptr) : device(dev) {}

        QString operator()(const QString& operation, const QVariantMap& config) const;
        QString operator()(const char* operation, const QVariantMap& config) const;

    private:
        S2VNADevice* device;
    };

    CommandGenerator generateCommand; // Теперь тип полный
    explicit S2VNADevice(int sens = 1, int sour = 1);
    QVariant parseResponse(const QString& response) override;

protected:
    friend class CommandGenerator; // Дружественный доступ
    QString buildFrequencySweep(const QVariantMap& config);

private:
    class ScpiBuilder {
    public:
        ScpiBuilder(int sens, int sour) :
            sens_prefix(QString(":SENS%1").arg(sens)),
            sour_prefix(QString(":SOUR%1").arg(sour)) {}

        QString freqStart(double value) const {
            return QString("%1:FREQ:STAR %2").arg(sens_prefix).arg(value);
        }

        QString powerLevel(double value) const {
            return QString("%1:POW:LEV %2dBm").arg(sour_prefix).arg(value);
        }

    private:
        QString sens_prefix;
        QString sour_prefix;
    };

    ScpiBuilder builder;
};

#endif // S2VNADEVICE_H
