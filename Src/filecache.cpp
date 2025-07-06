#include "filecache.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QList>
#include <QStandardPaths>

FileCache::FileCache( int cacheSize, QObject *parent)
    : QObject(parent),
    m_cacheSize(cacheSize)
{
    if (m_cacheSize <= 0 || m_cacheSize > 8) {
        m_cacheSize = 8;
         qDebug() << "Кэш был установлен дефолтным [максимальным] значением - 10 записей";
    }

    m_cacheDirPath = QDir::currentPath() + QDir::separator() + "history";

    QDir cacheDir(m_cacheDirPath);
    if (!cacheDir.exists()) {
        if (!cacheDir.mkpath(".")) {
            qWarning() << "Не удалось создать директорию для кэша:" << m_cacheDirPath;
        }
    }
}

void FileCache::getCacheFromResources()
{
    QDir cacheDir(m_cacheDirPath);

    if (!cacheDir.exists()) {
        qWarning() << "Директория для кэша:" << m_cacheDirPath << "не найдена.";
        return;
    }

    // Список файлов, отсортированных по дате изменения (от самых новых к самым старым)
    QList<QFileInfo> fileInfoList = cacheDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);

    m_cachedFiles.clear(); // Очищение текущего кэша
    for (int idx = 0; idx < qMin(fileInfoList.size(), m_cacheSize); ++idx) {
        m_cachedFiles.prepend(fileInfoList.at(idx).absoluteFilePath());
        qDebug() << "Загружен файл из кэша:" << fileInfoList.at(idx).absoluteFilePath();
    }

    // Удаление лишних файлов, если их больше, чем размер кэша (на случай если размер кэша уменьшили чужие рученки)
    while (m_cachedFiles.size() > m_cacheSize) {
        QString oldFile = m_cachedFiles.takeLast();
        QFile::remove(oldFile);
        qDebug() << "Удален лишний файл из кэша (превышен лимит):" << oldFile;
    }
    emit CacheUpdate(m_cachedFiles); // Оповещение UI о начальной загрузке кэша
}

void FileCache::saveDataToCache(const QVector<QPair<QPair<double, double>, double>>& parsedData) {
    QString filePath = createNewFile(parsedData);
    if (!filePath.isEmpty()) {
        addFileToCache(filePath);
    } else {
        qWarning() << "Не удалось сохранить данные в файл кэша.";
    }
}

QString FileCache::createNewFile(const QVector<QPair<QPair<double, double>, double>>& Data) {
    // Создание имени файлу, аргументами являются: [ год месяц день _ часы минуты секунды миллисекунды ]
    QString fileName = QString("graphic_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"));

    // Опредение пути для сохранения файла: в подпапке кэша
    QString filePath = m_cacheDirPath + QDir::separator() + fileName;

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
        m_cachedFiles.removeOne(filePath); // Удаление старого вхождения, чтобы добавить в начало
        qDebug() << "Файл уже в кэше, порядок изменен:" << filePath;
    } else {
        // Если кэш полон, удаление самого старого файла
        if (m_cachedFiles.size() >= m_cacheSize) {
            QString oldFile = m_cachedFiles.takeLast();
            QFile::remove(oldFile);
            qDebug() << "Кэш полон. Удаление самого старого файла из кэша:" << oldFile;
        }
    }
    m_cachedFiles.prepend(filePath);

    emit CacheUpdate(m_cachedFiles); // Оповещение для UI, что кэш обновлен
    qDebug() << "Файл: " << filePath << " добавлен/перемещен в начало кэша";
}


void FileCache::clearCache()
{
    // Проход по всем файлам в кэше и их удаление:
    for (const QString &filePath : m_cachedFiles) {
        if (QFile::remove(filePath)) {
            qDebug() << "Удален файл из кэша:" << filePath;
        } else {
            qWarning() << "Не удалось удалить файл из кэша:" << filePath;
        }
    }
    m_cachedFiles.clear();

    emit CacheUpdate(m_cachedFiles);
    qDebug() << "Весь кэш был очищен.";
}
