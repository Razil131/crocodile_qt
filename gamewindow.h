#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "paintwidget.h"
#include <QTimer>
#include "GameController.hpp"
#include "Player.hpp"

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
    QTimer *gameTimer;
    QTimer *wordTimer;
    int duration = 0;
    void setPlayer(Player* plr);
    Player getPlayer(){return player;}

private slots:
    void on_StartGameButton_clicked();

    void on_Word1Label_clicked();


    void on_BrushSizeSlider_valueChanged(int value);

    void on_ChooseColorButton_clicked();


    void on_FillingButton_clicked(bool checked);

    void on_EraseButton_pressed();

    void on_EraseButton_released();
    void on_EnterChat_released();

    void chatUpdate();

    void on_Word2Label_clicked();

    void on_Word3Label_clicked();

private:
    Ui::GameWindow *ui;
    PaintWidget *paintWidget;
    // struct ChatEntry{
    //     QString sender;
    //     QString message;
    // };
    QString getWordLabelStr(std::vector<std::string> letters);
    void startDraw();

    // QList<ChatEntry> chatHistory;

    QTimer *roundTimer;
    int timeLeft;
    GameController controller;
    Player player;



protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // GAMEWINDOW_H
