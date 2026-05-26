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
    std::vector<std::string> getWordsFromFile(const std::string& path);
    QString normalize(const QString& word);
    void openRandomLetter(GameState& state);
    QString chooseRandomWord();
    
public:
    bool updateOpenedLetters(GameState& state);
    void setCurrentWord(GameState& state,const QString& word);
    bool isWordCorrect(GameState& state, const QString& word);
    std::vector<QString> chooseWords();
};