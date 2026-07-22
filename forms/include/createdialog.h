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
    void setPort(const int port){port_=port;}
    int getPort(){return port_;}
    void setMaxClients(const int maxClients){maxClients_=maxClients;}
    int getMaxClients(){return maxClients_;}

private:
    Ui::CreateDialog *ui;
    int port_;
    int maxClients_;

signals:
    void nicknameEntered(const QString& nickname);
};

#endif // CREATEDIALOG_H