#include <gtest/gtest.h>
#include "RoundManager.hpp"
#include "GameState.hpp"
#include "Player.hpp"
#include <ctime>

class RoundManagerTest : public ::testing::Test {
protected:
    RoundManager roundManager;
    GameState state;

    void SetUp() override {
        Player p1(1, "ply1");
        Player p2(2, "ply2");
        Player p3(3, "ply3");

        state.addPlayer(p1);
        state.addPlayer(p2);
        state.addPlayer(p3);
    }
};

// startNewRound

TEST_F(RoundManagerTest, StartFirstRoundInitializesRoundCountAndIncrementsNum) {
    state.setRoundNum(0);
    
    for (auto& ply : state.mutablePlayers()) {
        ply.setIsCurrentWordGuessed(true);
    }

    roundManager.startNewRound(state);

    EXPECT_EQ(state.RoundCount(), 3);
    EXPECT_EQ(state.RoundNum(), 1);

    for (const auto& ply : state.players()) {
        EXPECT_FALSE(ply.isCurrentWordGuessed());
    }
}

TEST_F(RoundManagerTest, StartSubsequentRoundIncrementsNumAndResetsFlags) {
    state.setRoundCount(3);
    state.setRoundNum(1);

    for (auto& ply : state.mutablePlayers()) {
        ply.setIsCurrentWordGuessed(true);
    }

    roundManager.startNewRound(state);

    EXPECT_EQ(state.RoundNum(), 2);
    for (const auto& ply : state.players()) {
        EXPECT_FALSE(ply.isCurrentWordGuessed());
    }
}

TEST_F(RoundManagerTest, StartRoundWhenMaxRoundsReachedDoesNotIncrementNum) {
    state.setRoundCount(3);
    state.setRoundNum(3);

    roundManager.startNewRound(state);

    EXPECT_EQ(state.RoundNum(), 3);
}


// nextExplainer


TEST_F(RoundManagerTest, NextExplainerShiftsToNextPlayer) {
    state.setExplainerID(1);

    roundManager.nextExplainer(state);

    EXPECT_EQ(state.explainerID(), 2);
}

TEST_F(RoundManagerTest, NextExplainerWrapsAroundAtTheEnd) {
    state.setExplainerID(3);

    roundManager.nextExplainer(state);

    EXPECT_EQ(state.explainerID(), 1);
}

TEST_F(RoundManagerTest, NextExplainerHandlesEmptyPlayersGracefully) {
    GameState emptyState;
    emptyState.setExplainerID(99);

    roundManager.nextExplainer(emptyState);
    
    EXPECT_EQ(emptyState.explainerID(), 99);
}

// restartGame

TEST_F(RoundManagerTest, RestartGameResetsScoresRoundNumAndExplainer) {
    state.setRoundNum(5);
    state.setExplainerID(3);
    
    int scoreMultiplier = 100;
    for (auto& ply : state.mutablePlayers()) {
        ply.setScore(scoreMultiplier);
        scoreMultiplier += 100;
    }

    roundManager.restartGame(state);

    EXPECT_EQ(state.RoundNum(), 0);
    
    EXPECT_EQ(state.explainerID(), 1);
    roundManager.startNewRound(state);

    for (const auto& ply : state.players()) {
        EXPECT_EQ(ply.score(), 0);
    }
}