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

void CreateDialog::accept()
{
    QString nickname = ui->NickInput->text().trimmed();

    if (nickname.isEmpty()) {
        ui->NickInput->setPlaceholderText("Ник не может быть пустым!");
        return;
    }

    emit nicknameEntered(nickname);

    QDialog::accept();
}