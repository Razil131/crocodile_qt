#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QMetaObject>
#include <vector>

#include "GameController.hpp"
#include "Player.hpp"

class GameControllerTest : public ::testing::Test {
protected:
    std::unique_ptr<QCoreApplication> app;
    std::unique_ptr<GameController> gameController;

    void SetUp() override {
        int argc = 0;
        char* argv[] = { nullptr };
        app = std::make_unique<QCoreApplication>(argc, argv);
        
        gameController = std::make_unique<GameController>();
    }

    void TearDown() override {
        gameController.reset();
        app.reset();
    }
};

// ИНТЕГРАЦИЯ И СКЛЕЙКА СИГНАЛОВ (ПРОБРОС)

TEST_F(GameControllerTest, VerifiesSignalForwardingFromSubControllers) {
    bool playerAddedEmitted = false;
    int expectedId = 42;
    QString expectedName = "ply";

    QObject::connect(gameController.get(), &GameController::playerAdded,
        [&](int id, const QString& name) {
            playerAddedEmitted = true;
            EXPECT_EQ(id, expectedId);
            EXPECT_EQ(name, expectedName);
        });


    QMetaObject::invokeMethod(gameController->players(), "playerAdded",
                            Q_ARG(int, expectedId),
                            Q_ARG(QString, expectedName));

    EXPECT_TRUE(playerAddedEmitted);
}


// ПОДСЧЕТ ОЧКОВ ПРИ УГАДЫВАНИИ


TEST_F(GameControllerTest, PlayerGuessedWordUpdatesScoresForPlayerAndExplainer) {
    std::vector<int> capturedIds;
    int nominatedExplainerId = -1;

    QObject::connect(gameController.get(), &GameController::playerAdded,
        [&](int playerId, const QString& name) {
            capturedIds.push_back(playerId);
        });

    QObject::connect(gameController.get(), &GameController::explainerUpdated,
        [&](int newExplainerId) {
            nominatedExplainerId = newExplainerId;
        });

    gameController->players()->createAndAddPlayer("ply1");
    gameController->players()->createAndAddPlayer("ply2");
    app->processEvents();

    const auto& playersList = gameController->getPlayers();
    ASSERT_GE(playersList.size(), 2);

    int firstId = playersList[0].id();
    int secondId = playersList[1].id();

    gameController->round()->startWordChooseAndRound();
    app->processEvents();

    if (nominatedExplainerId == -1) {
        nominatedExplainerId = firstId;
    }
    int guesserId = (firstId == nominatedExplainerId) ? secondId : firstId;

    bool guesserScoreChanged = false;
    bool explainerScoreChanged = false;

    QObject::connect(gameController.get(), &GameController::playerScoreChanged,
        [&](int playerId, int newScore) {
            if (playerId == guesserId && newScore > 0) {
                guesserScoreChanged = true;
            }
            if (playerId == nominatedExplainerId && newScore > 0) {
                explainerScoreChanged = true;
            }
        });

    QMetaObject::invokeMethod(gameController->chat(), "playerGuessedWord",
                            Q_ARG(int, guesserId),
                            Q_ARG(int, 15));
    
    app->processEvents();


    EXPECT_TRUE(guesserScoreChanged);
    EXPECT_TRUE(explainerScoreChanged);
}

// ГЕТТЕРЫ-ФАСАДЫ


TEST_F(GameControllerTest, FacadeGettersReturnCorrectDefaultValues) {
    EXPECT_EQ(gameController->getRound(), 0); 
    EXPECT_GT(gameController->getRoundTime(), 0);
    EXPECT_TRUE(gameController->getOpenedLetters().empty());
    EXPECT_EQ(gameController->getWord(), "");
}