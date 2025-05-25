#include "s2vna_scpi.h"

QVariant S2VNA_SCPI::parseResponse(const QString& response)
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

QString S2VNA_SCPI::generateCommand(const QVariantMap& params) const
{
    QStringList commands;

    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        // Команда - ключ, Значение - строка
        // Формирование команды: "SCPI_CMD VALUE"
        if(it.value() != 0){
            commands.append(QString("%1 %2").arg(it.key(), it.value().toString()));
        } else{
            commands.append(it.key());
        }
    }
    // Объеденение команд ";\n":
    return commands.join("\n;") + "\n";
}

