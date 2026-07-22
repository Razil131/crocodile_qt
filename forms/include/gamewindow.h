#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPainter>
#include <QStyle>

#include "paintwidget.h"
#include "GameController.hpp"
#include "Player.hpp"

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum class MsgType {
        Info,
        Success,
        Warning
    };

    explicit GameWindow(GameController* ctrl, QWidget *parent = nullptr);
    ~GameWindow();

    void setPlayer(int assignedId);
    int getPlayerID() const { return playerId_; }
    void setHostMode(bool isHost);
    void updateIPlabel();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_StartGameButton_clicked();
    void on_Word1Label_clicked();
    void on_Word2Label_clicked();
    void on_Word3Label_clicked();
    void on_BrushSizeSlider_valueChanged(int value);
    void on_ChooseColorButton_clicked();
    void on_FillingButton_clicked(bool checked);
    void on_EraseButton_pressed();
    void on_EraseButton_released();
    void on_EnterChat_released();
    void onWordChooseStarted(const QString& w1, const QString& w2, const QString& w3);
    void onRoundStarted(int roundNum, const QString& wordToDraw);
    void onOpenedLettersUpdated(const QList<QString>& openedLetters);
    void onGameTimerUpdated(std::time_t timeLeft);
    void onWordTimerUpdated(std::time_t timeLeft);
    void onRoundEnded();
    void onExplainerUpdated(int newExplainerId);
    void onMessageReceived(int senderId, const QString& senderName, const QString& text);
    void onGameEnded();

private:
    QString getWordLabelStr(const QList<QString>& letters);
    void showRound();
    void playersTableUpdate();
    void tableCreate();
    void tableInChat();
    void addSystemMessage(const QString& text, MsgType type = MsgType::Info);

    Ui::GameWindow *ui;
    GameController* controller;

    QTimer *pressTimer;
    int duration = 0;
    int playerId_ = -1;
};

#endif // GAMEWINDOW_H
