#include "s2vna_scpi.h"

//вх. данные соотв. эксп. нотации: [действительная часть, мнимая часть]
//вых. данные: Вектор[  [действительная часть, мнимая часть], частота  ]
QVector<QPair<QPair<double, double>, double>> S2VNA_SCPI::parseResponse(const QString& response)
{
    QVector<QPair<QPair<double, double>, double>> sParams;
    QStringList parts = response.split(',');

    for(auto &el : parts){
        // удаление всех '+':
        el.replace("+", "");
    }

// Проверка четности количества параметров:
    if (parts.size() % 2 != 0) {
    // qWarning() << "Invalid response format!";
        return QVector<QPair<QPair<double, double>, double>>();
    }
    auto it_x = Frequency.constBegin();
    auto it_y = parts.constBegin();
    while (it_y != parts.constEnd()) {
    //================ [ [действительная часть, мнимая часть        ],  частота  ] ====//
        sParams.append({ {(it_y++)->toDouble(), (it_y++)->toDouble()}, *(it_x++) });
    }
    return sParams;
}

QString S2VNA_SCPI::generateCommand(const QList<QPair<QString, QVariant>>& params) const
{ // Команда - ключ, Значение - строка
    QStringList commands;

    for (const auto &[key, value] : params) {
        // Формирование команды: "SCPI_CMD VALUE" + учитывать отсутствие VALUE:
        commands.append(value.toInt() ? QString("%1 %2").arg(key, value.toString()) : key);
    }
    // Объеденение команд "\n;":
    return commands.join("\n;") + "\n";
}
