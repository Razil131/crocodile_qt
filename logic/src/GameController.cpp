#include "GameController.hpp"

GameController::GameController(){
    chat_ = ChatManager();
    state_ = GameState();
    wordmanager_ = WordManager();
    roundmanager_ = RoundManager();
}

void GameController::sendMessage(Player& ply, std::string message){
    if (ply.id() != state_.explainerID()){
        if (!ply.isCurrentWordGuessed()){
            chat_.addMessage(ply,message,state_,wordmanager_);
        }
    }
}

std::vector<std::pair<std::string,std::string>> GameController::getChatHistory(){
    return chat_.messages();
}

std::vector<Player> GameController::getPlayers(){
    return state_.players();
}

std::time_t GameController::getTimeLeft(){
    std::time_t time_left = state_.roundEndTime()-std::time(nullptr);
    return time_left >= 0 ? time_left : 0;
}

int GameController::getRoundTime(){
    return state_.ROUND_TIME;
}

std::vector<std::string> GameController::getOpenedLetters(){
    return state_.openedLetters();
}

std::string GameController::getIP(){
    return "255.255.255.255:65535"; // TODO
}

int GameController::getRound(){
    return state_.RoundNum();
}

int GameController::getRoundCount(){
    return state_.RoundCount();
}

bool GameController::isExplainer(const Player& ply){
    return ply.id() == state_.explainerID();
}

void GameController::setWord(std::string word){
    wordmanager_.setCurrentWord(state_, word, state_.ROUND_TIME);
}

std::tuple<std::string, std::string, std::string> GameController::getWordsForChoose(){
    return std::make_tuple(wordmanager_.chooseRandomWord(), wordmanager_.chooseRandomWord(), wordmanager_.chooseRandomWord());
}

void GameController::startRound(){
    roundmanager_.startNewRound(state_);
}

void GameController::updateOpenedLetters(){
    wordmanager_.updateOpenedLetters(state_);
}

void GameController::addPlayer(Player ply){
    state_.addPlayer(ply);
}

