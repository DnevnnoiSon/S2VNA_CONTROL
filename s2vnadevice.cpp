#include "s2vnadevice.h"

using namespace std;

QString S2VNADevice::generateCommand(const QString& operation, const QVariantMap& config)
{
    if (operation == "FREQ_SWEEP"){
        return buildFrequencySweep(config);
    }
    else if (operation == "SET_POWER"){
        return QString(":SOUR1:POW %1").arg(config["power"].toDouble());
    }
    return "";
}

QVariant S2VNADevice::parseResponse(const QString& response)
{
    QVector<QPair<double, double>> sParams;
    QStringList parts = response.split(',');

// Проверка четности количества параметров:
    if (parts.size() % 2 != 0) {
    // qWarning() << "Invalid response format!";
        return QVariant();
    }

    auto it = parts.constBegin();
    while (it != parts.constEnd()) {
        double re = (it++)->toDouble();
        double im = (it++)->toDouble();
        sParams.append({re, im});
    }
    return QVariant::fromValue(sParams);
}

QString S2VNADevice::buildFrequencySweep(const QVariantMap& config)
{
    QStringList commands;
    commands << QString(":SENS1:FREQ:START %1").arg(toScientific(config["start_freq"].toDouble()));
    commands << QString(":SENS1:FREQ:STOP %1").arg(toScientific(config["stop_freq"].toDouble()));
    return commands.join(";");
}
