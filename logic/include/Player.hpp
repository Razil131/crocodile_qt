#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <QObject>
#include "exportMacro.hpp"

class GameState;
class WordManager;

class CROCODILE_BACK_EXPORT Player{
private:
    int id_;
    QString name_;
    int score_;
    int guessedCount_;
    bool isCurrentWordGuessed_;
public:
    
    Player(int id, const QString& name);

    ~Player(){}

    int id() const { return id_; }
    int score() const { return score_; }
    int guessedCount() const { return guessedCount_; }
    void setScore(int score) { score_ = score; }
    void addScore(const int score);
    void addOneToGuessedCount() { guessedCount_++; }

    int calculateScoreForGuess(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const;
    int calculateScoreForDrawing(const std::time_t timeLeftTillRoundEnd, const int ROUND_TIME) const;
    
    const QString& name() const { return name_; }
    void setName(const QString &name) { name_ = name; }

    bool isCurrentWordGuessed() const { return isCurrentWordGuessed_; }
    void setIsCurrentWordGuessed(bool isCurrentWordGuessed) { isCurrentWordGuessed_ = isCurrentWordGuessed; }

};
