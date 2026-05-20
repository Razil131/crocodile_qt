#include "createdialog.h"
#include "ui_createdialog.h"

CreateDialog::CreateDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateDialog)
{
    ui->setupUi(this);
}

CreateDialog::~CreateDialog()
{
    delete ui;
}

void CreateDialog::on_buttonBox_accepted()
{
    QString nickname = ui->NickInput->text();
    if(!nickname.isEmpty()){
        Player* newPlayer = new Player(nickname.toStdString());
        emit PlayerCreated(newPlayer);
    }
}

