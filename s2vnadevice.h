#ifndef S2VNADEVICE_H
#define S2VNADEVICE_H

#include<complex>

#include"SCPIDevice.h"

class S2VNADevice : public SCPIDevice {
public:
    QString generateCommand(const QString& operation, const QVariantMap& config) override;
    QVariant parseResponse(const QString& response) override;

private:
    QString buildFrequencySweep(const QVariantMap& config);
};

#endif // S2VNADEVICE_H
