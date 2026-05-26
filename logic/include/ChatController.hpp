#pragma once

#include <QObject>
#include "Player.hpp"
#include "ChatManager.hpp"
#include "GameState.hpp"


class CROCODILE_BACK_EXPORT ChatController: public QObject{
    Q_OBJECT
signals:
    void chatUpdated();
    void messageReceived(int senderId, const QString& senderName, const QString& text);
    void playerGuessedWord(int playerId, int scoreBonus);
    void playerScoreMayHaveChanged();
    void openedLettersMayHaveChanged();
    void playersUpdated();

private:
    GameState& state_;
    WordManager& wordManager_;
    ChatManager chat_;
    bool canSendMessage(const Player& ply);

public:
    ChatController(GameState& state, WordManager& wordManager);
    void sendMessage(Player& ply, const QString& message);



};