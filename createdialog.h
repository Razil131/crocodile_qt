#ifndef CREATEDIALOG_H
#define CREATEDIALOG_H

#include <QDialog>
#include "Player.hpp"

namespace Ui {
class CreateDialog;
}

class CreateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDialog(QWidget *parent = nullptr);
    ~CreateDialog();

signals:
    void PlayerCreated(Player* player);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CreateDialog *ui;
};

#endif // CREATEDIALOG_H
