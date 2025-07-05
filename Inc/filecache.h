#ifndef FILECACHE_H
#define FILECACHE_H

#include <QObject>
#include <QQueue>
#include <QString>
#include <QDebug>
#include <QDir>
#include <QDateTime>

class FileCache : public QObject
{
   Q_OBJECT
public:
    explicit FileCache(int cacheSize = 10, QObject *parent = nullptr);

    //Создание файла - добавление туда данных
    static QString createNewRecord(const std::vector<std::pair<std::pair<double, double>, double>>& Data);

signals:
    void CacheUpdate(const QQueue<QString> &cachedFiles);
private:
    //Добавление имеющегося кэша с папки /Recourse в очередь
    void getCacheFromResources();

    // Вспомогательная функция для добавления файла в кэш с учетом лимита
    void addFileToCache(const QString &filePath);

    int m_cacheSize;
    QQueue<QString> m_cachedFiles;
};

#endif // FILECACHE_H
