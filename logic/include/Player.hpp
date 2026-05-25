#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include "exportMacro.hpp"

class GameState;
class WordManager;

class CROCODILE_BACK_EXPORT Player{
private:
    inline static int next_id_ = 1;
    int id_;
    std::string name_;
    int score_;
    int guessedCount_;
    bool isCurrentWordGuessed_;
public:
    
    Player(std::string name);

    ~Player(){}

    int id() const { return id_; }
    int score() const { return score_; }
    int guessedCount() const { return guessedCount_; }
    void setScore(int score) { score_ = score; }
    void addScore(const int score);
    void addOneToGuessedCount() { guessedCount_++; }

    int calculateScoreForGuess(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const;
    int calculateScoreForDrawing(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const;
    
    std::string name() const { return name_; }
    void setName(const std::string &name) { name_ = name; }

    bool isCurrentWordGuessed() const { return isCurrentWordGuessed_; }
    void setIsCurrentWordGuessed(bool isCurrentWordGuessed) { isCurrentWordGuessed_ = isCurrentWordGuessed; }

};
