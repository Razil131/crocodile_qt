#include "mainwindow.h"

#include <QApplication>
#include "paintwidget.h"
#include "gamewindow.h"

int main(int argc, char *argv[])
{/*
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QCoreApplication::exec();*/

    QApplication a(argc, argv);

    // Создаем виджет отдельно
    GameWindow w;
    w.resize(800, 600); // Задаем размер, так как у него нет родителя
    w.show();           // Вызываем окно отдельно

    return a.exec();
}
