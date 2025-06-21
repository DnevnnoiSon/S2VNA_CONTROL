#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "s2vna_scpi.h" // Добавлено для m_scpi

// Прямые объявления для уменьшения зависимостей в заголовках
namespace Ui {
class MainWindow;
}
class SParameterPlotter;
class ICommunication;
class ConnectionSettings;

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
    void handleIdnResponse(const QString &idnInfo);

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

    std::unique_ptr<Ui::MainWindow> ui; ///< Указатель на UI-компоненты, сгенерированные из .ui файла.
    SParameterPlotter* m_plotter;       ///< Указатель на виджет с графиком.
    std::unique_ptr<ICommunication> m_communicator; ///< Указатель на модуль связи.
    S2VNA_SCPI m_scpi;                  ///< Объект для генерации SCPI-команд.
};

#endif // MAINWINDOW_H
