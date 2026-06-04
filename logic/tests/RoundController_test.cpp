#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QMetaObject>
#include <ctime>
#include <vector>

#include "RoundController.hpp"
#include "GameState.hpp"
#include "WordManager.hpp"
#include "RoundManager.hpp"
#include "Player.hpp"

class RoundControllerTest : public ::testing::Test {
protected:
    std::unique_ptr<QCoreApplication> app;
    
    GameState state;
    WordManager realWordManager;
    RoundManager realRoundManager;
    std::unique_ptr<RoundController> controller;

    QTimer* gameTimer = nullptr;
    QTimer* wordTimer = nullptr;

    void SetUp() override {
        int argc = 0;
        char* argv[] = { nullptr };
        app = std::make_unique<QCoreApplication>(argc, argv);

        Player p1(1, "ply1");
        Player p2(2, "ply2");
        state.addPlayer(p1);
        state.addPlayer(p2);
        state.setExplainerID(1);

        controller = std::make_unique<RoundController>(state, realWordManager, realRoundManager);

        auto timers = controller->findChildren<QTimer*>();
        ASSERT_EQ(timers.size(), 2);
        gameTimer = timers[0];
        wordTimer = timers[1];
    }

    void TearDown() override {
        controller.reset();
        app.reset();
    }
};

// ВЫБОР СЛОВА И СТАРТ РАУНДА


TEST_F(RoundControllerTest, StartWordChooseEmitsSignalsAndStartsWordTimer) {
    bool signalEmitted = false;
    
    QObject::connect(controller.get(), &RoundController::wordsForChooseReady,
        [&](const QString& w1, const QString& w2, const QString& w3) {
            signalEmitted = true;
            EXPECT_FALSE(w1.isEmpty());
            EXPECT_FALSE(w2.isEmpty());
            EXPECT_FALSE(w3.isEmpty());
        });

    controller->startWordChooseAndRound();

    EXPECT_TRUE(signalEmitted);
    EXPECT_TRUE(wordTimer->isActive());
    EXPECT_FALSE(gameTimer->isActive());
}

TEST_F(RoundControllerTest, SetWordStartsGameTimerAndTriggersNewRound) {
    state.setRoundNum(0);
    bool signalEmitted = false;
    QString chosenWord;

    QObject::connect(controller.get(), &RoundController::wordsForChooseReady,
        [&](const QString& w1, const QString&, const QString&) {
            chosenWord = w1; 
        });
    controller->startWordChooseAndRound();

    QObject::connect(controller.get(), &RoundController::roundStarted,
        [&](int roundNum, const QString& word) {
            signalEmitted = true;
            EXPECT_EQ(roundNum, 1);
            EXPECT_EQ(word, chosenWord);
        });

    controller->setWord(chosenWord);

    EXPECT_TRUE(signalEmitted);
    EXPECT_FALSE(wordTimer->isActive());
    EXPECT_TRUE(gameTimer->isActive());
}


// ТАЙМЕРЫ (TICK EVENTS)

TEST_F(RoundControllerTest, WordTimerTimeoutForcesFirstWordSelection) {
    controller->startWordChooseAndRound();

    for (int i = 0; i < 9; ++i) {
        QMetaObject::invokeMethod(wordTimer, "timeout");
    }
    EXPECT_TRUE(wordTimer->isActive());

    QMetaObject::invokeMethod(wordTimer, "timeout");

    EXPECT_FALSE(wordTimer->isActive());
    EXPECT_TRUE(gameTimer->isActive()); 
}

TEST_F(RoundControllerTest, GameTickEmitsRemainingTime) {
    state.setRoundEndTime(std::time(nullptr) + 30);

    bool timerEmitted = false;
    QObject::connect(controller.get(), &RoundController::timerUpdated,
        [&](std::time_t timeLeft) {
            timerEmitted = true;
            EXPECT_GT(timeLeft, 0);
            EXPECT_LE(timeLeft, 30);
        });

    QMetaObject::invokeMethod(gameTimer, "timeout");

    EXPECT_TRUE(timerEmitted);
}


// ЗАВЕРШЕНИЕ РАУНДА И ИГРЫ

TEST_F(RoundControllerTest, GameTickTriggersEndOfTheGameWhenMaxRoundsReached) {
    state.setRoundEndTime(std::time(nullptr) - 5);
    
    state.setRoundNum(2);
    state.setRoundCount(2); 

    bool roundEndedEmitted = false;
    QObject::connect(controller.get(), &RoundController::roundEnded, [&]() { roundEndedEmitted = true; });

    bool gameEndedEmitted = false;
    QObject::connect(controller.get(), &RoundController::gameEnded, [&]() { gameEndedEmitted = true; });

    gameTimer->start(1000);
    ASSERT_TRUE(gameTimer->isActive());

    QMetaObject::invokeMethod(gameTimer, "timeout");

    EXPECT_FALSE(gameTimer->isActive());
    EXPECT_TRUE(roundEndedEmitted);
    EXPECT_TRUE(gameEndedEmitted);
    EXPECT_EQ(state.RoundNum(), 0);
}