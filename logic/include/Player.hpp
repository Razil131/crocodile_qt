#pragma once

#include <string>
#include <ctime>
#include "GameState.hpp"


class Player{
private:
    inline static int next_id_ = 1;
    int id_;
    std::string name_;
    int score_;
    int guessed_count_;

    void setScore(int score) { score_ = score; }
    void addScore(const int score);
public:
    
    Player(std::string name);

    ~Player(){}

    int getId() const { return id_; }
    std::string getName() const { return name_; }
    int getScore() const { return score_; }
    int guessedCount() const { return guessed_count_; }

    bool tryToGuessWord(GameState& gameState, const std::string word);
    int calculateScoreForGuess(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const;
};
