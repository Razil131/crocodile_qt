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

#ifdef UNIT_TESTS
#include <gtest/gtest_prod.h>
#else
#define FRIEND_TEST(test_case_name, test_name)
#endif

class CROCODILE_BACK_EXPORT WordManager{
    FRIEND_TEST(WordManagerTest, OpenRandomLetterDecrementsCountAndUpdatesMask);
    FRIEND_TEST(WordManagerTest, OpenRandomLetterEarlyReturnsWhenNoLettersToOpen);
    FRIEND_TEST(WordManagerTest, ChooseRandomWordReturnsFallbackWhenFileMissing);
private:
    QList<std::string> getWordsFromFile(const std::string& path);
    QString normalize(const QString& word);
    void openRandomLetter(GameState& state);
    QString chooseRandomWord();
    
public:
    bool updateOpenedLetters(GameState& state);
    void setCurrentWord(GameState& state,const QString& word);
    virtual bool isWordCorrect(GameState& state, const QString& word);
    QList<QString> chooseWords();
};
