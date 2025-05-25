#ifndef S2VNA_SCPI_H
#define S2VNA_SCPI_H

#include <QVariant>
#include <QVariantMap>
#include <QString>
#include <QStringList>

class S2VNA_SCPI
{ /* [1] - Парсинг scpi, [2] - Сборка scpi, [3] - Парсинг ответа от *IDN? */
public:
    QVariant parseResponse(const QString& response);          // [1]
    QString generateCommand(const QVariantMap& params) const; // [2]
//    QVariant parse_IDNResponse(const QString &idn_response);  // [3]
};

#endif // S2VNA_SCPI_H
