#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QChart>
#include <QVector>

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
private:
    Ui::MainWindow *ui;
 //Обьект коммуникации:
    ICommunication* communicator;
//Устройство SCPI: [S2VNA]
    S2VNA_SCPI scpi;

    QChart *chart;
};

#endif // MAINWINDOW_H
