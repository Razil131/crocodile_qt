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
    int port;
    int maxClients;
    
    explicit CreateDialog(QWidget *parent = nullptr);
    ~CreateDialog();
    void accept() override;

private:
    Ui::CreateDialog *ui;

signals:
    void nicknameEntered(const QString& nickname);
};

#endif // CREATEDIALOG_H