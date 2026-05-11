#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "paintwidget.h"
#include <QTimer>

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();
    QTimer *pressTimer;
    int duration = 0;

private slots:
    void on_StartGameButton_clicked();

    void on_Word1Label_clicked();


    void on_BrushSizeSlider_valueChanged(int value);

    void on_ChooseColorButton_clicked();


    void on_FillingButton_clicked(bool checked);

    void on_EraseButton_pressed();

    void on_EraseButton_released();
private:
    Ui::GameWindow *ui;
    PaintWidget *paintWidget;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // GAMEWINDOW_H
