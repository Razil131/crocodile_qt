#pragma once

#include <vector>
#include <tuple>
#include <string>
#include "Player.hpp"
#include "GameState.hpp"
#include "WordManager.hpp"


class ChatManager{
private:
    std::vector<std::pair<std::string,std::string>> messages_;
    bool tryGuess(Player& ply,GameState& gameState, WordManager& wordManager, const std::string& word);
public:
    void addMessage(Player& ply, std::string& message, GameState& gameState, WordManager& wordManager);
    std::vector<std::pair<std::string,std::string>> messages () const {return messages_;}
};