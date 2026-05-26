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

void JoinDialog::accept()
{
    QString nickname = ui->NickInput->text().trimmed();

    if (nickname.isEmpty()) {
        ui->NickInput->setPlaceholderText("Ник не может быть пустым!");
        return;
    }

    emit nicknameEntered(nickname);

    QDialog::accept();
}