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
    if (c_dialog.exec() == QDialog::Accepted) {
        GameWindow *g = new GameWindow(this);
        g->show();
    }
}


void MainWindow::on_connectButton_clicked()
{
    JoinDialog j_dialog(this);
    if (j_dialog.exec() == QDialog::Accepted) {
        GameWindow *g = new GameWindow(this);
        g->show();
    }
}

