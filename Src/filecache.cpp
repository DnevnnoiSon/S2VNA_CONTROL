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
    if (m_cacheSize <= 0 || m_cacheSize > 10) {
        m_cacheSize = 10;
        // qWarning() << "Кэш был установлен дефолтным [максимальным] значением - 10 записей"; // Это не предупреждение, а ожидаемое поведение
    }

    // Определяем путь для папки кэша. Используем QStandardPaths для надежности.
    // Например, в пользовательских данных приложения. Или просто в текущей директории.
    // Выберем текущую директорию для простоты демонстрации, создав подпапку "cache".
    m_cacheDirPath = QDir::currentPath() + QDir::separator() + "cache";

    QDir cacheDir(m_cacheDirPath);
    if (!cacheDir.exists()) {
        if (!cacheDir.mkpath(".")) { // Создаем папку, если ее нет
            qWarning() << "Не удалось создать директорию кэша:" << m_cacheDirPath;
        }
    }
}

void FileCache::getCacheFromResources()
{
    QDir cacheDir(m_cacheDirPath);

    if (!cacheDir.exists()) {
        qWarning() << "Директория кэша:" << m_cacheDirPath << "не найдена. Возможно, не была создана.";
        return;
    }

    // Получаем список файлов, отсортированных по дате изменения (от самых новых к самым старым)
    QList<QFileInfo> fileInfoList = cacheDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);

    m_cachedFiles.clear(); // Очищаем текущий кэш перед заполнением
    for (int idx = 0; idx < qMin(fileInfoList.size(), m_cacheSize); ++idx) {
        m_cachedFiles.prepend(fileInfoList.at(idx).absoluteFilePath());
        qDebug() << "Загружен файл из кэша:" << fileInfoList.at(idx).absoluteFilePath();
    }

    // Удаление лишних файлов, если их больше, чем размер кэша (может быть, если размер кэша уменьшили)
    while (m_cachedFiles.size() > m_cacheSize) {
        QString oldFile = m_cachedFiles.takeLast();
        QFile::remove(oldFile);
        qDebug() << "Удален лишний файл из кэша (превышен лимит):" << oldFile;
    }
    emit CacheUpdate(m_cachedFiles); // Оповещаем UI о начальной загрузке кэша
}

void FileCache::saveDataToCache(const QVector<QPair<QPair<double, double>, double>>& parsedData) {
    QString filePath = createNewRecord(parsedData);
    if (!filePath.isEmpty()) {
        addFileToCache(filePath);
    } else {
        qWarning() << "Не удалось сохранить данные в файл кэша.";
    }
}

QString FileCache::createNewRecord(const QVector<QPair<QPair<double, double>, double>>& Data) {
    // Создание имени файлу, аргументами являются: [ год месяц день _ часы минуты секунды миллисекунды ]
    QString fileName = QString("graph_data_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz"));

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
        m_cachedFiles.removeOne(filePath); // Удаляем старое вхождение, чтобы добавить в начало
        qDebug() << "Файл уже в кэше, порядок изменен:" << filePath;
    } else {
        // Если кэш полон, удаляем самый старый файл
        if (m_cachedFiles.size() >= m_cacheSize) {
            QString oldFile = m_cachedFiles.takeLast();
            QFile::remove(oldFile); // Удаляем файл с диска
            qDebug() << "Кэш полон. Удаление самого старого файла из кэша:" << oldFile;
        }
    }
    // Добавление в начало очереди:
    m_cachedFiles.prepend(filePath);

    emit CacheUpdate(m_cachedFiles); // Оповещение для UI, что кэш обновлен
    qDebug() << "Файл: " << filePath << " добавлен/перемещен в начало кэша";
}
