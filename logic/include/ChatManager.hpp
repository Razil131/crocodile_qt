#pragma once

#include <vector>
#include <tuple>
#include <string>
#include "Player.hpp"
#include "GameState.hpp"
#include "WordManager.hpp"
#include "exportMacro.hpp"


class CROCODILE_BACK_EXPORT ChatManager{
private:
    QList<std::pair<QString,QString>> messages_;
    bool tryGuess(Player& ply,GameState& gameState, WordManager& wordManager, const QString& word);
public:
    bool addMessage(Player& ply, const QString& message, GameState& gameState, WordManager& wordManager);
    const QList<std::pair<QString,QString>>& messages() const {return messages_;}
};