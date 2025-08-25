#include "mainwindow.h"
#include "connectionSettings.h"

#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Регистрация кастомного типа данных:
    qRegisterMetaType<ConnectionSettings>("ConnectionSettings");


    MainWindow w;
    w.show();

    return a.exec();
}
