#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <cctype>

#include "Player.hpp"

class GameState
{
public:
    static const int ROUND_TIME = 60*1; //DEBUG было 60*5

private://TODO количество раундов надо сделать равное количеству игроков в игре чтобы каждый ведущим побыл
    std::string currentWord_;
    std::vector<std::string> openedLetters_;
    std::time_t roundEndTime_;
    int alreadyOpenedLetters_;
    int countLettersToOpen_;
    int letterTimeInterval_;
    std::vector<Player> players_;
    Player* explainer_;

    std::string normalize(std::string str);
    std::vector<std::string> getWordsFromFile(std::string fileName);
public:

    GameState();

    ~GameState(){}

    std::string chooseRandomWord();

    bool isWordCorrect(const std::string word);

    void updateOpenedLetters();

    void openRandomLetter();

    void chooseRandomExplainer();

    std::string currentWord() const { return currentWord_; }

    std::time_t getRoundEndTime() const {return roundEndTime_;}

    std::vector<std::string> openedLetters() const { return openedLetters_; }

    std::vector<Player> players() const { return players_; }
    void addPlayer(const Player ply) { players_.push_back(ply); }

    Player* explainer() const { return explainer_; }
    void setExplainer(Player* explainer) { explainer_ = explainer; }

};