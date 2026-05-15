#pragma once

#include "GameState.hpp"
#include "ChatManager.hpp"

class GameController{
private:
    GameState state_;
    ChatManager chat_;
public:
    GameController();
    void sendMessage(Player ply, std::string message);
    std::vector<std::pair<std::string,std::string>> getChatHistory();
    std::vector<Player> getPlayers();
    std::time_t getTimeLeft();
    int getRoundTime();
    std::vector<std::string> getOpenedLetters();
    std::string getIP();
    int getRound();
    bool isExplainer(Player* ply);
};
