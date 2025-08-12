#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

#include <QObject>
#include "connectionSettings.h"

/**
 * @brief Интерфейс для модуля связи с устройством.
 * @details Определяет базовый контракт для всех классов, реализующих
 * взаимодействие с измерительным оборудованием.
 * Абстрагирует способы подключения (сокет, COM-порт и т.д.).
 */
class ICommunication : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса ICommunication.
     * @param parent Родительский объект QObject.
     */
    explicit ICommunication(QObject *parent = nullptr) : QObject(parent) {}

    /**
     * @brief Виртуальный деструктор.
     */
    virtual ~ICommunication() = default;

    /**
     * @brief Инициирует подключение к устройству.
     */
    virtual void connectToDevice() = 0;

    /**
     * @brief Отправляет команду на устройство.
     * @param command Строка с SCPI-командой.
     * @return 0 в случае успеха, иначе код ошибки.
     */
    virtual int sendCommand(const QString &command) = 0;

    /**
     * @brief Запускает периодический опрос состояния устройства.
     * @details Используется для проверки доступности устройства и его статуса.
     */
    virtual void startPolling() = 0;

    /**
     * @brief Останавливает периодический опрос состояния.
     */
    virtual void stopPolling() = 0;

public slots:
    /**
     * @brief Слот для приема конфигурации измерений от GUI.
     * @param command SCPI-команда с параметрами измерения.
     */
    virtual void acceptMeasureConfig(const QString& command) = 0;

    /**
     * @brief Слот для приема настроек подключения от GUI.
     * @param setting Структура с параметрами подключения (IP, порт и т.д.).
     */
    virtual void acceptSettingConfig(const ConnectionSettings &setting) = 0;

signals:
    /**
     * @brief Сигнал изменения статуса подключения к устройству.
     * @param isReady true, если устройство готово к работе, иначе false.
     */
    void deviceStatusChanged(bool isReady);

    /**
     * @brief Сигнал об ошибке, возникшей в модуле связи.
     * @param message Текстовое описание ошибки.
     */
    void errorOccurred(const QString& message);

    /**
     * @brief Сигнал, передающий идентификационную информацию об устройстве.
     * @param idnInfo Строка, полученная в ответ на команду *IDN?.
     */
    void idnReceived(const QString &idnInfo);

    /**
     * @brief Сигнал, передающий полученные S-параметры.
     * @param response Строка с данными S-параметров от устройства.
     */
    void sParamsReceived(const QString &response);
};

#endif // ICOMMUNICATION_H
