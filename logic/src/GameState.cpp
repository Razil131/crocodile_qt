#include "GameState.hpp"

GameState::GameState() :  
    roundEndTime_(std::time(nullptr) + ROUND_TIME),
    explainerID_(1),
    roundNum_(0),
    roundCount_(0),
    roundInProgress_(false)
{}

Player& GameState::explainer() {
    for (Player& ply:players_){
        if (ply.id() == explainerID_){
            return ply;
        }
    }
    std::cerr << "There is no player with explainer index";
}

void GameState::clearAndResizeOpenedLetters(int size) {
    openedLetters_.clear();
    openedLetters_.resize(size, "_");
}
