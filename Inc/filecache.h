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

    // Создание файла - теперь нестатический метод
    QString createNewRecord(const QVector<QPair<QPair<double, double>, double>>& Data);

public slots:
    // Добавление слота для сохранения данных, вызываемого из другого потока
    void saveDataToCache(const QVector<QPair<QPair<double, double>, double>>& parsedData);
    // Инициализация кэша из ресурсов/файлов при запуске
    void getCacheFromResources();

signals:
    // Сигнал обновления элементов очереди -> требуется обновить панель
    void CacheUpdate(const QQueue<QString> &cachedFiles);

private:
    // Вспомогательная функция для добавления файла в кэш с учетом лимита
    void addFileToCache(const QString &filePath);

    int m_cacheSize;
    QQueue<QString> m_cachedFiles;
    QString m_cacheDirPath;
};

#endif // FILECACHE_H
