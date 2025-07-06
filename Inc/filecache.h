#ifndef FILECACHE_H
#define FILECACHE_H

#include <QObject>
#include <QQueue>
#include <QString>
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QVector>
#include <QPair>

/**
 * @class FileCache
 * @brief Управляет кэшем файлов данных, хранящихся на диске.
 *
 * Класс занимается обрабаткой создания, хранения и извлечения кэшированных файлов
 * Использует директорию на диске для сохранения кэша между запусками приложения.
 */
class FileCache : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса FileCache.
     * @param cacheSize Максимальное количество файлов для хранения в кэше. По умолчанию 10.
     * @param parent Родительский объект QObject. По умолчанию nullptr.
     */
    explicit FileCache(int cacheSize = 10, QObject *parent = nullptr);

    /**
     * @brief Создает новый файл записи в кэше из предоставленных данных.
     * @param Data - данные для графика предоставленные к сохранению.
     * @details Данные представляют собой пары состоящие из S-параметров [действительная и мнимая части] и значений частоты.
     * @return Абсолютный путь к созданному файлу или пустую строку QString в случае ошибки.
     */
    QString createNewFile(const QVector<QPair<QPair<double, double>, double>>& Data);

public slots:
    /**
     * @brief Публичный слот для сохранения данных в новый файл кэша.
     *
     * Этот слот предназначен для вызова из другого потока,
     * для нового файла в кэше с предоставленными данными
     * @param parsedData Данные для сохранения.
     */
    void saveDataToCache(const QVector<QPair<QPair<double, double>, double>>& parsedData);

    /**
     * @brief Инициализирует кэш, загружая существующие файлы из директории кэша.
     *
     * Сканирует директорию кэша, загружает самые последние файлы в пределах
     * лимита размера кэша и заполняет внутреннюю очередь кэша.
     */
    void getCacheFromResources();

    /**
     * @brief Очищает весь кэш.
     * @details Удаляет все файлы из директории кэша и очищает внутреннюю очередь.
     * Испускает сигнал CacheUpdate с пустой очередью.
     */
    void clearCache();

signals:
    /**
     * @brief Сигнал, который испускается при каждом обновлении содержимого кэша.
     *
     * Испускается после добавления нового файла или при первоначальной загрузке кэша,
     * позволяя пользовательскому интерфейсу или другим компонентам обновиться.
     * @param cachedFiles Обновленная очередь путей к кэшированным файлам, где самый новый находится в начале.
     */
    void CacheUpdate(const QQueue<QString> &cachedFiles);

private:
    /**
     * @brief Добавляет путь к файлу в кэш, управляя лимитом размера кэша.
     *
     * Если файл уже находится в кэше, он перемещается в начало.
     * Если кэш полон, самый старый файл удаляется как из кэша, так и с диска.
     * @param filePath Абсолютный путь к файлу для добавления.
     */
    void addFileToCache(const QString &filePath);

    int m_cacheSize;  ///< Максимальное количество файлов, которое может содержать кэш
    QString m_cacheDirPath;        ///< Путь к директории кэша
    QQueue<QString> m_cachedFiles; ///< Очередь абсолютных путей к кэшированным файлам

};

#endif // FILECACHE_H
