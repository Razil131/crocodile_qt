#ifndef CREATEDIALOG_H
#define CREATEDIALOG_H

#include <QDialog>
#include <QString>

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
    int port;

signals:
    void nicknameEntered(const QString& nickname);

private:
    Ui::CreateDialog *ui;
};

#endif // CREATEDIALOG_H