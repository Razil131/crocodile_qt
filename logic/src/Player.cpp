#include "Player.hpp"

Player::Player(std::string name): name_(name){
    id_=next_id_;
    next_id_++;
    score_ = 0;
    guessed_count_ = 0;
}

void Player::addScore(const int score){
    score_ += score;
}

int Player::calculateScoreForGuess(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const{
    return std::floor((timeLeftTillRoundEnd/(ROUND_TIME*1.0))*1000);
}

bool Player::tryToGuessWord(GameState& gameState, const std::string word){
    bool isCorrect = gameState.isWordCorrect(word);
    gameState.updateOpenedLetters();
    if (isCorrect){
        guessed_count_++;
        std::time_t curTime = std::time(nullptr);
        std::time_t timeLeftTillRoundEnd = gameState.getRoundEndTime() - curTime;
        addScore(calculateScoreForGuess(timeLeftTillRoundEnd,gameState.ROUND_TIME));
    }
    return isCorrect;
}
