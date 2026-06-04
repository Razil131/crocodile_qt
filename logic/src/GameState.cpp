#include "GameState.hpp"

GameState::GameState() :  
    roundEndTime_(std::time(nullptr) + ROUND_TIME),
    explainerID_(1),
    roundNum_(0),
    roundCount_(3),
    roundInProgress_(false),
    alreadyOpenedLetters_(0),
    countLettersToOpen_(0),
    letterTimeInterval_(0)
{}

Player& GameState::explainer() {
    for (Player& ply:players_){
        if (ply.id() == explainerID_){
            return ply;
        }
    }
    throw std::runtime_error("There is no player with explainer index: " + std::to_string(explainerID_));
}

void GameState::clearAndResizeOpenedLetters(int size) {
    openedLetters_.clear();
    openedLetters_.resize(size, "_");
}
