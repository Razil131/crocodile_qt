#include "Player.hpp"
#include "GameState.hpp"
#include "WordManager.hpp"

Player::Player(int id, const QString& name)
    : id_(id)
    , name_(name)
    , score_(0)
    , guessedCount_(0)
    , isCurrentWordGuessed_(false){}

void Player::addScore(const int score){
    score_ += score;
}

int Player::calculateScoreForGuess(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const{
    return std::floor((timeLeftTillRoundEnd/(ROUND_TIME*1.0))*1000);
}

int Player::calculateScoreForDrawing(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const{
    return std::floor((timeLeftTillRoundEnd/(ROUND_TIME*1.0))*200);
}
