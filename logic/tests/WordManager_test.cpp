#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ctime>
#include <fstream>
#include "WordManager.hpp"
#include "GameState.hpp"

class WordManagerTest : public ::testing::Test {
protected:
    WordManager wm;
    GameState state;

    void createFakeDictionary(const std::string& path, const std::vector<std::string>& words) {
        std::ofstream file(path);
        for (const auto& word : words) {
            file << word << "\n";
        }
        file.close();
    }

    void removeFakeDictionary(const std::string& path) {
        std::remove(path.c_str());
    }
};

// НОРМАЛИЗАЦИЯ И ПРОВЕРКА СЛОВ


TEST_F(WordManagerTest, IsWordCorrectHandlesDifferentCasesAndSpaces) {
    state.setCurrentWord("crocodile");

    EXPECT_TRUE(wm.isWordCorrect(state, "crocodile"));
    
    EXPECT_TRUE(wm.isWordCorrect(state, "CroCoDiLe"));
    
    EXPECT_TRUE(wm.isWordCorrect(state, "  crocodile  "));
    
    EXPECT_FALSE(wm.isWordCorrect(state, "alligator"));
}

// SETCURRENTWORD (РАСЧЕТ ИНТЕРВАЛОВ И БУКВ)

TEST_F(WordManagerTest, SetCurrentWordInitializesStateCorrectly) {
    wm.setCurrentWord(state, "  Crocodile  ");

    EXPECT_EQ(state.currentWord(), "crocodile");

    EXPECT_EQ(state.alreadyOpenedLetters(), 0);

    EXPECT_EQ(state.countLettersToOpen(), 5);

    EXPECT_EQ(state.letterTimeInterval(), 45);

    ASSERT_EQ(state.openedLetters().size(), 9);
    for (const auto& letter : state.openedLetters()) {
        EXPECT_EQ(letter, "_");
    }
}

// OPENRANDOMLETTER

TEST_F(WordManagerTest, OpenRandomLetterDecrementsCountAndUpdatesMask) {
    wm.setCurrentWord(state, "crocodile");
    int initialCount = state.countLettersToOpen();

    wm.openRandomLetter(state);

    EXPECT_EQ(state.countLettersToOpen(), initialCount - 1);

    int openedCount = 0;
    for (const auto& letter : state.openedLetters()) {
        if (letter != "_") openedCount++;
    }
    EXPECT_EQ(openedCount, 1);
}

TEST_F(WordManagerTest, OpenRandomLetterEarlyReturnsWhenNoLettersToOpen) {
    wm.setCurrentWord(state, "crocodile");
    state.setCountLettersToOpen(0);

    auto initialLetters = state.openedLetters();
    wm.openRandomLetter(state);

    EXPECT_EQ(state.openedLetters(), initialLetters);
}


// ОБНОВЛЕНИЕ БУКВ ПО ТАЙМЕРУ (UPDATEOPENEDLETTERS)


TEST_F(WordManagerTest, UpdateOpenedLettersDoesNotOpenIfTimeNotPassed) {
    wm.setCurrentWord(state, "crocodile"); 
    
    // прошло 0 секунд с начала раунда
    state.setRoundEndTime(std::time(nullptr) + state.ROUND_TIME);

    bool opened = wm.updateOpenedLetters(state);
    
    EXPECT_FALSE(opened);
    EXPECT_EQ(state.alreadyOpenedLetters(), 0);
}

TEST_F(WordManagerTest, UpdateOpenedLettersOpensMultipleLettersWhenTimePassed) {
    wm.setCurrentWord(state, "crocodile");
    state.setLetterTimeInterval(10);

    // прошло 25 секунд с начала раунда
    state.setRoundEndTime(std::time(nullptr) + state.ROUND_TIME - 25);

    bool opened = wm.updateOpenedLetters(state);

    EXPECT_TRUE(opened);
    EXPECT_EQ(state.alreadyOpenedLetters(), 2);
    
    int openCount = 0;
    for (const auto& l : state.openedLetters()) {
        if (l != "_") openCount++;
    }
    EXPECT_EQ(openCount, 2);
}

TEST_F(WordManagerTest, UpdateOpenedLettersReturnsFalseIfRoundEnded) {
    wm.setCurrentWord(state, "crocodile");
    
    state.setRoundEndTime(std::time(nullptr) - 5);

    EXPECT_FALSE(wm.updateOpenedLetters(state));
}

// ЗАГРУЗКА ФАЙЛОВ


TEST_F(WordManagerTest, ChooseRandomWordReturnsFallbackWhenFileMissing) {
    QString word = wm.chooseRandomWord();
    EXPECT_FALSE(word.isEmpty());
}

TEST_F(WordManagerTest, ChooseWordsReturnsExactlyThreeWords) {
    std::vector<QString> choices = wm.chooseWords();
    EXPECT_EQ(choices.size(), 3);
    for (const auto& word : choices) {
        EXPECT_FALSE(word.isEmpty());
    }
}