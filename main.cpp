#include "mainwindow.h"
#include "gamewindow.h"
#include "joindialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    JoinDialog joinDialog;
    GameWindow gameWindow;

    QObject::connect(&joinDialog, &JoinDialog::PlayerCreated,
                     &gameWindow, &GameWindow::setPlayer);


    return QCoreApplication::exec();
}
