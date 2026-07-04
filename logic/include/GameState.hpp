#pragma once

#include <cctype>
#include <random>
#include <iostream>
#include <vector>
#include <QObject>
#include <QDataStream>
#include <QList>
#include "Player.hpp"
#include "exportMacro.hpp"
#include "RoundManager.hpp"



class CROCODILE_BACK_EXPORT GameState
{
public:
    static const int ROUND_TIME = 60*3;

private:
    std::time_t roundEndTime_;
    QList<Player> players_;
    int explainerID_;
    int roundNum_;
    int roundCount_;
    bool roundInProgress_;
    QString currentWord_;
    QList<QString> openedLetters_;
    int alreadyOpenedLetters_;
    int countLettersToOpen_;
    int letterTimeInterval_;
public:

    GameState();

    ~GameState(){}

    std::time_t roundEndTime() const {return roundEndTime_;}
    const QList<Player>& players() const { return players_; }
    QList<Player>& mutablePlayers() { return players_; }
    void addPlayer(const Player& ply) { players_.push_back(ply); }

    int explainerID() const { return explainerID_; }
    Player& explainer();
    void setExplainerID(int explainerID) { explainerID_ = explainerID; }
    
    int RoundNum() const {return roundNum_;}
    int RoundCount() const {return roundCount_;}
    bool isRoundInProgress() const {return roundInProgress_;}

    void setRoundInProgress(bool roundInProgress) { roundInProgress_ = roundInProgress; }
    void setRoundCount(int roundCount) { roundCount_ = roundCount; }
    void setRoundNum(int roundNum) { roundNum_ = roundNum; }

    void setRoundEndTime(const std::time_t &roundEndTime) { roundEndTime_ = roundEndTime; }

    int letterTimeInterval() const { return letterTimeInterval_; }
    void setLetterTimeInterval(int letterTimeInterval) { letterTimeInterval_ = letterTimeInterval; }

    int countLettersToOpen() const { return countLettersToOpen_; }
    void setCountLettersToOpen(int countLettersToOpen) { countLettersToOpen_ = countLettersToOpen; }

    int alreadyOpenedLetters() const { return alreadyOpenedLetters_; }
    void setAlreadyOpenedLetters(int alreadyOpenedLetters) { alreadyOpenedLetters_ = alreadyOpenedLetters; }

    const QList<QString>& openedLetters() const { return openedLetters_; }
    void setOpenedLetters(const QList<QString> &openedLetters) { openedLetters_ = openedLetters; }

    const QString currentWord() const { return currentWord_; }
    void setCurrentWord(const QString &currentWord) { currentWord_ = currentWord; }

    void clearAndResizeOpenedLetters(int size);
};

CROCODILE_BACK_EXPORT QDataStream &operator<<(QDataStream &out, const GameState &state);
CROCODILE_BACK_EXPORT QDataStream &operator>>(QDataStream &in, GameState &state);