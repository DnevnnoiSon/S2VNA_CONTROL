#include "s2vna_scpi.h"
#include <QDebug> // Добавлено для qWarning

//вх. данные соотв. эксп. нотации: [действительная часть, мнимая часть]
// sParams - [действительная часть, мнимая часть]
// Frecuency - [ действительная часть ]
//вых. данные: Вектор[ sParams , Frecuency ]
QVector<QPair<QPair<double, double>, double>> S2VNA_SCPI::parseResponse(const QString& response, const QVector<double>& frequency)
{
    QVector<QPair<QPair<double, double>, double>> coordinates; // Переименовано с 'Сoordinates'
    QStringList parts = response.split(',');
    //==== !ОСТОРОЖНО! - может сломать эксп. нотацию
    for(auto &el : parts){
        // удаление всех '+':
        el.replace("+", "");
    }

    if( (parts.size() / 2) != frequency.size() ){
        qWarning() << "Frequency size is " << frequency.size();
        qWarning() << "Sparams size is " << parts.size() / 2;
        return coordinates;
    }

    // Проверка четности количества параметров:
    if (parts.size() % 2 != 0 ) {
        qWarning() << "Invalid response format!";
        return coordinates;
    }
    auto it_x = frequency.constBegin();
    auto it_y = parts.constBegin();
    while (it_y != parts.constEnd()) {
        //================ [ [действительная часть, мнимая часть        ],  частота  ] ====//
        coordinates.append({ {(it_y++)->toDouble(), (it_y++)->toDouble()}, *(it_x++) });
    }
    return coordinates;
}

QString S2VNA_SCPI::generateCommand(const QList<QPair<QString, QVariant>>& params) const
{ // Команда - ключ, Значение - строка
    QStringList commands;

    for (const auto &[key, value] : params) {
        // Формирование команды: "SCPI_CMD VALUE" + учитывать отсутствие VALUE:
        if (value.isValid() && !value.toString().isEmpty()) {
            if (value.typeId() == QMetaType::Int && value.toInt() == 0 && key.endsWith("?")) {
                commands.append(key);
            } else {
                commands.append(QString("%1 %2").arg(key, value.toString()));
            }
        } else {
            commands.append(key);
        }
    }
    // Объеденение команд ";":
    return commands.join("\n;") + "\n";
}
