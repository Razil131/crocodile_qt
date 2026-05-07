#include "gamewindow.h"
#include "ui_gamewindow.h"

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
{
    ui->setupUi(this);
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::on_StartGameButton_clicked()
{
    ui->StartGameButton->hide();
}


void GameWindow::on_Word1Label_clicked()
{
    ui->Word1Label->hide();
    ui->Word2Label->hide();
    ui->Word3Label->hide();
    ui->TimeToChooseWordLabel->hide();
}

