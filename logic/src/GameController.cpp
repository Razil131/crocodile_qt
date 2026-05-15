#include "GameController.hpp"

GameController::GameController(){
    chat_ = ChatManager();
    state_ = GameState();
}

void GameController::sendMessage(Player ply, std::string message){
    ply.tryToGuessWord(state_, message);
    chat_.addMessage(ply,message);
}

std::vector<std::pair<std::string,std::string>> GameController::getChatHistory(){
    return chat_.messages();
}

std::vector<Player> GameController::getPlayers(){
    return state_.players();
}

std::time_t GameController::getTimeLeft(){
    return state_.getRoundEndTime()-std::time(nullptr);
}

int GameController::getRoundTime(){
    return state_.ROUND_TIME;
}

std::vector<std::string> GameController::getOpenedLetters(){
    return state_.openedLetters();
}

std::string GameController::getIP(){
    return "255.255.255.255:65535"; // DEBUG
}

int GameController::getRound(){
    return 1; // DEBUG
}

bool GameController::isExplainer(Player* ply){
    return ply == state_.explainer();
}