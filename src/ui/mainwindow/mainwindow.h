#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "s2vnascpi.h"

#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class QThread;

class SParameterPlotter;
class ICommunication;
class ConnectionSettings;
class FileCache;

/**
 * @brief Главное окно приложения.
 * @details Отвечает за компоновку UI, обработку действий пользователя и
 * взаимодействие между модулем связи и модулем отрисовки графика.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна.
     * @param parent Родительский виджет.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Деструктор.
     */
    ~MainWindow();

signals:
    /**
     * @brief Сигнал для передачи конфигурации измерения в модуль связи.
     * @param command Готовая SCPI-команда.
     */
    void measureConfigTransferred(const QString &command);

    /**
     * @brief Сигнал для передачи новых настроек подключения в модуль связи.
     * @param setting Структура с новыми настройками.
    */
    void settingConfigTransferred(const ConnectionSettings &setting);

    /**
     * @brief Сигнал для асинхронного запроса данных из файла истории.
     * @param fileName Имя файла для чтения.
    */
    void requestHistoryData(const QString &fileName);
private slots:
    /**
     * @brief Слот, обрабатывающий нажатие на кнопку "Измерить".
     */
    void on_measureButton_clicked();

    /**
     * @brief Слот, обрабатывающий нажатие на кнопку "Обновить" (настройки сети).
     */
    void on_updateButton_clicked();

    /**
     * @brief Обновляет UI в соответствии со статусом подключения.
     * @param isReady true, если устройство готово.
     */
    void onDeviceStatusChanged(bool isReady);

    /**
     * @brief Отображает сообщение об ошибке в строке состояния.
     * @param errorMessage Текст ошибки.
     */
    void handleDeviceError(const QString& errorMessage);

    /**
     * @brief Обрабатывает и отображает идентификационную информацию устройства.
     * @param idnInfo Ответ от устройства на команду *IDN?.
     */
    void handleIdnResponse( const QString &idnInfo);

    /**
     * @brief Обрабатывает обновление кэша
     * @param cachedFiles Актуальные файлы находящиеся в обновленном кэше.
     */
    void updateCacheListView(const QQueue<QString> &cachedFiles);

    /**
     * @brief Обрабатывает нажатие на кнопку в кэше
     * @param Имя файла для открытия и считывания
     */
    void onHistoryButtonClicked(const QString &fileName);

    /**
     * @brief Слот, обрабатывающий нажатие на кнопку "Удалить историю".
     */
    void on_deleteHistoryPushButton_clicked();

    /**
     * @brief Слот для получения данных из кэша и отрисовки графика.
     * @param response Строка с S-параметрами.
     * @param frequencies Вектор частот.
    */
    void onHistoryDataReady(const QString &response, const QVector<double> &frequencies);
private:
    /**
     * @brief Инициализирует и настраивает компоненты UI.
     */
    void InitUI();

    /**
     * @brief Устанавливает все необходимые сигнально-слотовые соединения.
     */
    void setupConnections();

    /**
     * @brief Применяет стили и начальные настройки к виджетам.
     */
    void setupUiAppearance();

    std::unique_ptr<Ui::MainWindow> ui;  ///< Указатель на UI-компоненты, сгенерированные из .ui файла.
    SParameterPlotter* m_plotter;        ///< Указатель на виджет с графиком.

    ICommunication* m_communicator;  ///< Указатель на модуль связи.
    FileCache* m_fileCache;          ///< Указатель на модуль кэширования [файловое управление кэшем]

    QThread* m_commThread;  ///< Поток, в котором будет работать модуль связи.
    QThread* m_cacheThread;  ///< Поток в котором будет работать модуль кэширования

    S2VNA_SCPI m_scpi;                   ///< Объект для генерации SCPI-команд.

    QStandardItemModel* m_cacheModel;    ///< Указатель на QListView
};

#endif // MAINWINDOW_H
