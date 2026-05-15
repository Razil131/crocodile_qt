#pragma once

#include <string>
#include <ctime>
#include <cmath>

class GameState;

class Player{
private:
    inline static int next_id_ = 1;
    int id_;
    std::string name_;
    int score_;
    int guessedCount_;

    void setScore(int score) { score_ = score; }
    void addScore(const int score);
public:
    
    Player(std::string name);

    ~Player(){}

    int id() const { return id_; }
    int score() const { return score_; }
    int guessedCount() const { return guessedCount_; }

    bool tryToGuessWord(GameState& gameState, const std::string word);
    int calculateScoreForGuess(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const;
    int calculateScoreForDrawning(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const;
    
    std::string name() const { return name_; }
    void setName(const std::string &name) { name_ = name; }
};
