#pragma once
#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <ctime>
#include "exportMacro.hpp"

#include "GameState.hpp"


class CROCODILE_BACK_EXPORT WordManager{
private:
    std::vector<std::string> getWordsFromFile(std::string path);
    std::string normalize(std::string word);
    void openRandomLetter(GameState& state);
public:
    std::string chooseRandomWord();
    bool updateOpenedLetters(GameState& state);
    void setCurrentWord(GameState& state, std::string word, const int ROUND_TIME);
    bool isWordCorrect(GameState& state, const std::string word);
};