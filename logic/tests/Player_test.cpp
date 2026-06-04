#include <gtest/gtest.h>
#include "Player.hpp"

TEST(PlayerTest, ConstructorInitializesFieldsCorrectly) {
    Player player(42, "ply");

    EXPECT_EQ(player.id(), 42);
    EXPECT_EQ(player.name(), "ply");
    EXPECT_EQ(player.score(), 0);
    EXPECT_EQ(player.guessedCount(), 0);
    EXPECT_FALSE(player.isCurrentWordGuessed());
}

TEST(PlayerTest, GettersAndSettersWork) {
    Player player(1, "ply");

    player.setName("ply");
    EXPECT_EQ(player.name(), "ply");

    player.setIsCurrentWordGuessed(true);
    EXPECT_TRUE(player.isCurrentWordGuessed());

    player.setIsCurrentWordGuessed(false);
    EXPECT_FALSE(player.isCurrentWordGuessed());
}

TEST(PlayerTest, ScoreManipulation) {
    Player player(1, "ply");

    player.setScore(100);
    EXPECT_EQ(player.score(), 100);

    player.addScore(50);
    EXPECT_EQ(player.score(), 150);

    player.addScore(-20);
    EXPECT_EQ(player.score(), 130);
}

TEST(PlayerTest, IncrementsGuessedCount) {
    Player player(1, "ply");

    ASSERT_EQ(player.guessedCount(), 0);

    player.addOneToGuessedCount();
    EXPECT_EQ(player.guessedCount(), 1);

    player.addOneToGuessedCount();
    EXPECT_EQ(player.guessedCount(), 2);
}

TEST(PlayerTest, CalculateScoreForGuess) {
    Player player(1, "ply");
    const int ROUND_TIME = 180;

    int maxScore = player.calculateScoreForGuess(ROUND_TIME, ROUND_TIME);
    
    int midScore = player.calculateScoreForGuess(90, ROUND_TIME);
    
    int minScore = player.calculateScoreForGuess(1, ROUND_TIME);

    EXPECT_GE(maxScore, midScore);
    EXPECT_GE(midScore, minScore);
    
    EXPECT_GE(minScore, 0);
}


TEST(PlayerTest, CalculateScoreForDrawing) {
    Player player(1, "ply");
    const int ROUND_TIME = 180;

    int scoreAtStart = player.calculateScoreForDrawing(170, ROUND_TIME);
    int scoreAtEnd = player.calculateScoreForDrawing(10, ROUND_TIME);

    EXPECT_GT(scoreAtStart, scoreAtEnd);

    EXPECT_GE(scoreAtStart, 0);
    EXPECT_GE(scoreAtEnd, 0);
}