#include "filecache.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QList>

FileCache::FileCache( int cacheSize, QObject *parent)
    : QObject(parent),
    m_cacheSize(cacheSize)
{
    if (m_cacheSize <= 0 || m_cacheSize > 10) {
        m_cacheSize = 10;
        qWarning() << "Кэш был установлен дефолтным [максимальным] значением - 10 записей" << m_cacheSize;
    }
    getCacheFromResources(); // Просмотр папки --> [/Resources], поиск имеющихся там файлов
}

void FileCache::getCacheFromResources()
{
    QDir resourcesDir(":/Resources");

    if (!resourcesDir.exists()) {
        qWarning() << "Директорий кэша: ':/Resources' не был найден";
        return;
    }

    // Получаем список файлов, отсортированных по дате изменения (от самых новых к самым старым)
    QList<QFileInfo> fileInfoList = resourcesDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);

    for (int idx = 0; idx < qMin(fileInfoList.size(), m_cacheSize); ++idx) {
        m_cachedFiles.prepend(fileInfoList.at(idx).absoluteFilePath());
        qDebug() << "Загружен файл из ресурсов:" << fileInfoList.at(idx).absoluteFilePath();
    }
// Удаление лишних файлов в случае присутствия в директории /Resources лишних рук:
    while (m_cachedFiles.size() > m_cacheSize) {
        m_cachedFiles.takeLast();
    }
    emit CacheUpdate(m_cachedFiles); // Оповещаем UI о начальной загрузке кэша
}


QString FileCache::createNewRecord(const std::vector<std::pair<std::pair<double, double>, double>>& Data) {
    // Cоздание имени файлу, аргументами являются: [ год месяц день _ часы минуты секунды миллисекунды ]
    QString fileName = QString("graph_data_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"));

    // Опредение пути для сохранения файла:
    QString filePath = QDir::currentPath() + QDir::separator() + fileName;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
       // Запись данных:
        for (const auto& point : Data) {
            // Sparam(действительная, мнимая часть) - первое измерение
            // Frequency - второе измерение
            out << point.first.first << ", " << point.first.second << "   " << point.second << "\n";
        }
        file.close();
        qDebug() << "Создан новый файл записи:" << filePath;
        return filePath;
    }
    else{
        qWarning() << "Не удалось создать файл:" << filePath << "Ошибка:" << file.errorString();
        return QString(); // Возвращение пустой строки в случае ошибки
    }
}

void FileCache::addFileToCache(const QString &filePath){
// В случае если файл уже имеется:
    if (m_cachedFiles.contains(filePath)) {
        m_cachedFiles.removeOne(filePath);
        qDebug() << "Файл уже в кэше, порядок изменен:" << filePath;
    }

    if (m_cachedFiles.size() >= m_cacheSize) {
        QString oldFile = m_cachedFiles.takeLast();
        QFile::remove(oldFile);
        qDebug() << "Кэш полон. Удаление самого старого файла из кэша:" << oldFile;
    }
// Добавление в начало очереди:
    m_cachedFiles.prepend(filePath);

    emit CacheUpdate(m_cachedFiles); // Оповещение для UI, что кэш обновлен
    qDebug() << "Файл: " << filePath << " перемещен в начало кэша";
}

