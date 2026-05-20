#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
    GameWindow *g = new GameWindow(this);
    connect(&c_dialog, &CreateDialog::PlayerCreated,
            g, &GameWindow::setPlayer);
    if (c_dialog.exec() == QDialog::Accepted) {
        g->show();
    }
    else delete g;
}


void MainWindow::on_connectButton_clicked()
{
    JoinDialog j_dialog(this);
    GameWindow *g = new GameWindow(this);
    connect(&j_dialog, &JoinDialog::PlayerCreated,
            g, &GameWindow::setPlayer);
    if (j_dialog.exec() == QDialog::Accepted) {
        g->show();
    }
    else delete g;
}

