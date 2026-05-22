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
    void accept() override;

signals:
    void PlayerCreated(Player* player);

private slots:


private:
    Ui::CreateDialog *ui;
};

#endif // CREATEDIALOG_H
