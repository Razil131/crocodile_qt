#include "joindialog.h"
#include "ui_joindialog.h"

JoinDialog::JoinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::JoinDialog)
{
    ui->setupUi(this);
}

JoinDialog::~JoinDialog()
{
    delete ui;
}

void JoinDialog::on_buttonBox_accepted()
{
    QString nickname = ui->NickInput->text();
    if(!nickname.isEmpty()){
        Player* newPlayer = new Player(nickname.toStdString());
        emit PlayerCreated(newPlayer);
    }
}

