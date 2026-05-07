#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <ctime>

class GameState
{
public:
    static const int ROUND_TIME = 60*1; //DEBUG было 60*5

private:
    std::string currentWord_;
    std::vector<std::string> openedLetters_;
    std::time_t roundEndTime_;
    int alreadyOpenedLetters_;
    int countLettersToOpen_;
    int letterTimeInterval_;

    std::vector<std::string> getWordsFromFile(std::string fileName);
public:

    GameState();

    ~GameState(){}

    std::string chooseRandomWord();

    bool isWordCorrect(const std::string word) const;

    void updateOpenedLetters();

    void openRandomLetter();

    std::string currentWord() const { return currentWord_; }

    std::time_t getRoundEndTime() const {return roundEndTime_;}

    std::vector<std::string> openedLetters() const { return openedLetters_; }
    
};