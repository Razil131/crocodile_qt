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
