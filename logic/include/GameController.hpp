#pragma once
#include <QObject>
#include <memory>
#include "exportMacro.hpp"
#include "GameState.hpp"
#include "WordManager.hpp"
#include "RoundManager.hpp"

#include "ChatController.hpp"
#include "DrawController.hpp"
#include "PlayerController.hpp"
#include "RoundController.hpp"

class CROCODILE_BACK_EXPORT GameController : public QObject {
    Q_OBJECT
private:
    GameState state_;
    WordManager wordmanager_;
    RoundManager roundmanager_;

    ChatController* chatController_;
    DrawController* drawController_;
    PlayerController* playerController_;
    RoundController* roundController_;

    std::time_t getTimeLeft();

signals:
    void playerAdded(int playerId, const QString& name);
    void playersUpdated();
    void playerScoreChanged(int playerId, int newScore);

    void messageReceived(int senderId, const QString& senderName, const QString& text);
    void chatUpdated();

    void drawCommandReceived(DrawCommand cmd);

    void explainerUpdated(int newExplainerId);
    void wordTimerUpdated(std::time_t timeLeft);
    void roundStarted(int roundNum, const QString& wordToDraw);
    void roundEnded();
    void timerUpdated(std::time_t timeLeft);
    void openedLettersUpdated(const QList<QString>& openedLetters);
    void wordsForChooseReady(const QString& w1, const QString& w2, const QString& w3);
    void gameEnded();

public:
    GameController();

    ChatController* chat() const { return chatController_; }
    DrawController* draw() const { return drawController_; }
    PlayerController* players() const { return playerController_; }
    RoundController* round() const { return roundController_; }

    const QList<std::pair<QString, QString>>& getChatHistory() const;
    const QList<Player>& getPlayers();
    int getRoundTime();
    QList<QString> getOpenedLetters();
    QString getIP();
    int getRound();
    int getRoundCount();
    QString getWord();
};