#pragma once

#include <string>
#include <ctime>
#include <compare>
#include <cmath>
#include <QObject>
#include <QDataStream>
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
    Player() = default;
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
    void setID(const int &id) { id_ = id; }
    bool isCurrentWordGuessed() const { return isCurrentWordGuessed_; }
    void setIsCurrentWordGuessed(bool isCurrentWordGuessed) { isCurrentWordGuessed_ = isCurrentWordGuessed; }

};
// вывод
inline QDataStream &operator<<(QDataStream &out, const Player &player) {

    out << qint32(player.id()) 
        << player.name() 
        << qint32(player.score());
    return out;
}

// ввод
inline QDataStream &operator>>(QDataStream &in, Player &player) {
    qint32 id, score;
    QString name;
    
    in >> id >> name >> score;

    player.setID(id);
    player.setName(name);
    player.setScore(score);
    return in;
}