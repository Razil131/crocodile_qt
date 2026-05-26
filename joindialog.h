#ifndef JOINDIALOG_H
#define JOINDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class JoinDialog;
}

class JoinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinDialog(QWidget *parent = nullptr);
    ~JoinDialog();
    void accept() override;

signals:
    void nicknameEntered(const QString& nickname);

private:
    Ui::JoinDialog *ui;
};

#endif // JOINDIALOG_H