#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ctime>
#include "GameState.hpp"
#include "Player.hpp"

class GameStateTest : public ::testing::Test {
protected:
    GameState state;
    
    Player p1{1, "ply1"};
    Player p2{2, "ply2"};
    Player p3{99, "ply3"};
};


// ИНИЦИАЛИЗАЦИЯ И ДЕФОЛТНОЕ СОСТОЯНИЕ


TEST_F(GameStateTest, ConstructorInitializesFieldsCorrectly) {
    EXPECT_EQ(state.explainerID(), 1);
    EXPECT_EQ(state.RoundNum(), 0);
    EXPECT_EQ(state.RoundCount(), 3);
    EXPECT_FALSE(state.isRoundInProgress());
    
    std::time_t now = std::time(nullptr);
    EXPECT_GE(state.roundEndTime(), now + GameState::ROUND_TIME - 2);
    EXPECT_LE(state.roundEndTime(), now + GameState::ROUND_TIME + 2);
}

TEST_F(GameStateTest, ConstructorInitializesUnmappedFields) {
    EXPECT_EQ(state.alreadyOpenedLetters(), 0);
    EXPECT_EQ(state.countLettersToOpen(), 0);
    EXPECT_EQ(state.letterTimeInterval(), 0);
    EXPECT_TRUE(state.openedLetters().empty());
    EXPECT_TRUE(state.currentWord().isEmpty());
}


// ЛОГИКА ИГРОКОВ И EXPLAINER

TEST_F(GameStateTest, AddPlayersAndRetrieveThem) {
    EXPECT_TRUE(state.players().empty());
    
    state.addPlayer(p1);
    state.addPlayer(p2);
    
    ASSERT_EQ(state.players().size(), 2);
    EXPECT_EQ(state.players()[0].id(), 1);
    EXPECT_EQ(state.players()[1].id(), 2);
}

TEST_F(GameStateTest, ExplainerReturnsCorrectPlayerReference) {
    state.addPlayer(p1);
    state.addPlayer(p2);
    state.addPlayer(p3);
    
    state.setExplainerID(1);
    EXPECT_EQ(state.explainer().id(), 1);
    EXPECT_EQ(state.explainer().name(), "ply1");
    
    state.setExplainerID(99);
    EXPECT_EQ(state.explainer().id(), 99);
    EXPECT_EQ(state.explainer().name(), "ply3");
}

TEST_F(GameStateTest, ExplainerHandlesMissingPlayerGracefully) {
    state.addPlayer(p2); 
    state.setExplainerID(1); 
    
    EXPECT_THROW(state.explainer(), std::runtime_error);
}

TEST_F(GameStateTest, MutablePlayersAllowsModifications) {
    state.addPlayer(p1);
    
    std::vector<Player>& mutable_p = state.mutablePlayers();
    ASSERT_EQ(mutable_p.size(), 1);
    
    mutable_p[0].setScore(500);
    
    EXPECT_EQ(state.players()[0].score(), 500);
}


// РАБОТА С БУКВАМИ И СТРОКАМИ


TEST_F(GameStateTest, ClearAndResizeOpenedLettersCorrectly) {
    state.clearAndResizeOpenedLetters(5);
    
    const auto& letters = state.openedLetters();
    ASSERT_EQ(letters.size(), 5);
    for (const QString& letter : letters) {
        EXPECT_EQ(letter, "_");
    }
}

TEST_F(GameStateTest, ClearAndResizeHandlesZeroAndOverwrite) {
    std::vector<QString> customLetters = {"A", "B", "C"};
    state.setOpenedLetters(customLetters);
    
    state.clearAndResizeOpenedLetters(0);
    EXPECT_TRUE(state.openedLetters().empty());
    
    state.clearAndResizeOpenedLetters(2);
    ASSERT_EQ(state.openedLetters().size(), 2);
    EXPECT_EQ(state.openedLetters()[0], "_");
    EXPECT_EQ(state.openedLetters()[1], "_");
}

TEST_F(GameStateTest, CurrentWordSetAndGet) {
    state.setCurrentWord("Crocodile");
    EXPECT_EQ(state.currentWord(), "Crocodile");
    
    state.setCurrentWord("");
    EXPECT_TRUE(state.currentWord().isEmpty());
}

TEST_F(GameStateTest, SettersAndGettersBoundaryValues) {
    state.setRoundNum(-1); 
    EXPECT_EQ(state.RoundNum(), -1); 

    state.setRoundCount(5);
    EXPECT_EQ(state.RoundCount(), 5);

    state.setAlreadyOpenedLetters(42);
    EXPECT_EQ(state.alreadyOpenedLetters(), 42);

    state.setCountLettersToOpen(3);
    EXPECT_EQ(state.countLettersToOpen(), 3);

    state.setLetterTimeInterval(15);
    EXPECT_EQ(state.letterTimeInterval(), 15);
    
    std::time_t customTime = 1234567890;
    state.setRoundEndTime(customTime);
    EXPECT_EQ(state.roundEndTime(), customTime);
    
    state.setRoundInProgress(true);
    EXPECT_TRUE(state.isRoundInProgress());
}