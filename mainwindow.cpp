#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "createdialog.h"
#include "joindialog.h"
#include "ui_joindialog.h"
#include "gamewindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createButton_clicked()
{
    CreateDialog c_dialog(this);
    GameWindow *g = new GameWindow(&controller, this);

    connect(&c_dialog, &CreateDialog::nicknameEntered, this, [this, g](const QString& nickname) {
        Player& newPlayer = controller.players()->createAndAddPlayer(nickname);
        g->setPlayer(newPlayer.id());
    });

    if (c_dialog.exec() == QDialog::Accepted) {
        controller.startNetworkServer(c_dialog.port);
        g->show();
    }
    else {
        delete g;
    }
}

void MainWindow::on_connectButton_clicked()
{
    JoinDialog j_dialog(this);
    GameWindow *g = new GameWindow(&controller, this);
    QString capturedNickname;
    connect(&j_dialog, &JoinDialog::nicknameEntered, this, [&capturedNickname](const QString& nickname) {
        capturedNickname = nickname;
    });
    connect(&controller, &GameController::localPlayerIdAssigned, g, &GameWindow::setPlayer);

    if (j_dialog.exec() == QDialog::Accepted) {
        controller.connectToNetworkServer(j_dialog.IP, j_dialog.port, capturedNickname);
        g->show();
    }
    else {
        delete g;
    }
}