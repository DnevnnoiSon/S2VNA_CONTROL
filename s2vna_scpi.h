#ifndef S2VNA_SCPI_H
#define S2VNA_SCPI_H

#include <QVariant>
#include <QVariantMap>
#include <QString>
#include <QStringList>

class S2VNA_SCPI
{ /* [1] - Парсинг scpi, [2] - Сборка scpi */
public:
    QVector<QPair<double, double>> parseResponse(const QString& response);          // [1]
    QString generateCommand(const QVariantMap& params) const; // [2]
};

#endif // S2VNA_SCPI_H
