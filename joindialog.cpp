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


void JoinDialog::on_buttonBox_accepted()
{
    IP = ui->IPInput1->text().trimmed() + "." +
         ui->IPInput2->text().trimmed() + "." +
         ui->IPInput3->text().trimmed() + "." +
         ui->IPInput4->text().trimmed();
    port = ui->PortSpinBox->value();
}

