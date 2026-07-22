#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Крокодил"));
    setWindowIcon(QIcon(":/assets/icon.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createButton_clicked()
{
    CreateDialog c_dialog(this);

    QPointer<GameController> localController = new GameController();
    QPointer<GameWindow> g = new GameWindow(localController, this);

    g->setAttribute(Qt::WA_DeleteOnClose);
    localController->setParent(g);

    connect(&c_dialog, &CreateDialog::nicknameEntered, this, [localController, g](const QString& nickname) {
        Player& newPlayer = localController->players()->createAndAddPlayer(nickname);
        g->setPlayer(newPlayer.id());
    });

    if (c_dialog.exec() == QDialog::Accepted) {
        localController->setMaxClients(c_dialog.getMaxClients());
        localController->startNetworkServer(c_dialog.getPort());
        g->show();
        g->setHostMode(true);
        localController->setPort(c_dialog.getPort());
        g->updateIPlabel();
    }
    else {
        delete g;
    }
}

void MainWindow::on_connectButton_clicked()
{
    JoinDialog j_dialog(this);
    QString capturedNickname;
    connect(&j_dialog, &JoinDialog::nicknameEntered, this, [&capturedNickname](const QString& nickname) {
        capturedNickname = nickname;
    });

    if (j_dialog.exec() != QDialog::Accepted) return;

    QPointer<GameController> localController = new GameController();
    QPointer<GameWindow> g = new GameWindow(localController, this);
    g->setAttribute(Qt::WA_DeleteOnClose);
    localController->setParent(g);

    connect(localController, &GameController::localPlayerIdAssigned, g, &GameWindow::setPlayer);

    connect(localController, &GameController::connectionSucceeded, this, [g]() {
        g->setHostMode(false);
        g->show();
    });

    localController->connectToNetworkServer(j_dialog.IP, j_dialog.port, capturedNickname);
    connect(localController, &GameController::connectionFailed, this, [g](const QString& reason) {
        g->deleteLater();
    });

    connect(localController, &GameController::serverDisconnected, this, [g] {
        g->deleteLater();
    });
}