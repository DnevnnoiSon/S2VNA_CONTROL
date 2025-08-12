#include "socketcommunication.h"
#include <QHostAddress>
#include <QThread>
#include <QEventLoop>

SocketCommunication::SocketCommunication(QObject *parent): ICommunication(parent)
{

}

void SocketCommunication::initialize()
{
    m_socket = std::make_unique<QTcpSocket>();
    m_pollTimer = std::make_unique<QTimer>();

    connect(m_pollTimer.get(), &QTimer::timeout, this, &SocketCommunication::attemptConnection);

    connect(m_socket.get(), &QTcpSocket::connected, this, &SocketCommunication::onConnected);
    connect(m_socket.get(), &QTcpSocket::readyRead, this, &SocketCommunication::onReadyRead);
    connect(m_socket.get(), &QTcpSocket::errorOccurred, this, &SocketCommunication::onErrorOccurred);

    startPolling();
}

void SocketCommunication::attemptConnection() {
    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        m_socket->connectToHost(m_targetAddress, m_port);
        // Состояние подключения обработается в слотах onConnected/onErrorOccurred.
    }
}

int SocketCommunication::sendCommand(const QString &command)
{
    //Проверка сетевого подключения:
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState){
        emit errorOccurred("Устройство не подключено");
        return 1;
    }
    qDebug() << "Отправка команды:" << command;
    const QByteArray scpi_cmd = command.toUtf8();


    if (m_socket->write(scpi_cmd) == -1) {
        emit errorOccurred("Ошибка записи в сокет");
        return 1;
    }

    if (!m_socket->waitForBytesWritten(WRITE_TIMEOUT_MS)){
        emit errorOccurred("Не удалось отправить команду [тайм-аут]");
        return 1;
    }
    return 0;
}

void SocketCommunication::connectToDevice(){
    // Момент первичного успешного подключения.
    sendCommand("*RST\n");
}

void SocketCommunication::onConnected(){   /* успешное подключение */
    stopPolling();
    isExpectingIDN = true;
    qDebug() << "Успешное сокет соединение";

    // Отправка команды идиентификации:
    sendCommand("*IDN?\n");
    emit deviceStatusChanged(true);
}

void SocketCommunication::onReadyRead() {     /* Готовность чтения[прием данных] */
    QString response;
    m_responseBuffer += m_socket->readAll();
    // Проверка на завершенность ответа - [\n]
    while (m_responseBuffer.contains('\n'))
    {
        int newline_pos = m_responseBuffer.indexOf('\n');
        QByteArray messageData = m_responseBuffer.left(newline_pos);
        // Удаление сообщения и символа '\n':
        m_responseBuffer.remove(0, newline_pos + 1);

        QString response = QString::fromUtf8(messageData);
        qDebug() << "Получен полный ответ:" << response;

        if (isExpectingIDN) {
            isExpectingIDN = false;
            emit idnReceived(response);
        }
        else if (!response.isEmpty()) {
            emit sParamsReceived(response);
        }
    }
    /// p.s. если неоконченное '\n' требуется кэширование, а может и просто ошибка:
}

void SocketCommunication::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    emit errorOccurred(m_socket->errorString());
    emit deviceStatusChanged(false);
    startPolling(); // Попытка переподключится
}

void SocketCommunication::acceptMeasureConfig(const QString &command) {
    if (command.isEmpty()) {
        emit errorOccurred("Ошибка: данные отстутствуют ");
        return;
    }
    qDebug() << "Строка которая будет парсится: " << command;

    QStringList parts = command.split(';');

    std::for_each(parts.begin(), parts.end() - 1, [this](const QString &part) {
        sendCommand(part);
    });

    QTimer::singleShot(350, this, [this, parts]() {  ///< Отказоустойчивость задержки на 1000 точек:
        sendCommand(parts.last());
    });
    /// trimmed() - не использовать, опасно для используемого формата команд
}

//Отправка валидных UI настроек модулю связи:
void SocketCommunication::acceptSettingConfig(const ConnectionSettings &setting) {
    stopPolling();

    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
     /// --- ОСОБЕННОСТЬ ВНУТРЕННЕЙ РЕАЛИЗАЦИИ SOCKET ---
        m_socket->abort(); // Используем abort() вместо disconnectFromHost()
    }

    if (setting.network.ip_addr.isEmpty() || setting.network.port == 0) {
        // ... (обработка ошибки) ...
        return;
    }

    QHostAddress newAddress(setting.network.ip_addr);
    m_targetAddress = newAddress;
    m_port = setting.network.port;

    startPolling();
}


void SocketCommunication::startPolling() {
    if (m_pollTimer && !m_pollTimer->isActive()) {
        m_pollTimer->start(POLLING_INTERVAL_MS);
    }
}

void SocketCommunication::stopPolling() {
    if (m_pollTimer) {
        m_pollTimer->stop();
    }
}
