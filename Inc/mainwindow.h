#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QLabel>
#include <QChart>
#include <QVector>

#include "sparameterplotter.h"

#include "icommunication.h"
#include "s2vna_scpi.h"
#include "ValidSettings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void transfer_measure_config(const QString &command);
    void transfer_setting_config(const Settings &setting);
private slots:
    void on_measureButton_clicked();
    void on_updateButton_clicked();
    void onDeviceStatusChanged(bool isReady);
    void handleDeviceError(const QString& errorMessage);

    void handleIdnResponse(const QString &idnInfo);
private:
//========= Конструкторские функции:
    void InitUI();           // Создание виджетов
    void ApplyStyles();      // Стилизация
    void SetupConnections(); // Сигналы/слоты
//========= Обьекты:
    Ui::MainWindow *ui;
// Ответ на IDN? - вывод на экран:
    QLabel *deviceInfoLabel[2];
 //Обьект коммуникации:
    ICommunication* communicator;
//Обрабатываемое устройство SCPI: [S2VNA]
    S2VNA_SCPI scpi;
//Построение графика:
    SParameterPlotter *plotter;
};

#endif // MAINWINDOW_H
