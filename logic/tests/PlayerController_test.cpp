#include <gtest/gtest.h>
#include <QtTest/QSignalSpy>
#include "PlayerController.hpp"
#include "GameState.hpp"
#include "Player.hpp"

TEST(PlayerControllerTest, IdentifiesExplainerCorrectly) {
    GameState state;
    PlayerController controller(state);

    state.setExplainerID(42);

    Player player1(42, "Explainer");
    Player player2(10, "ply");

    EXPECT_TRUE(controller.isExplainer(player1));
    EXPECT_FALSE(controller.isExplainer(player2));

    EXPECT_TRUE(controller.isExplainerByID(42));
    EXPECT_FALSE(controller.isExplainerByID(10));
}

TEST(PlayerControllerTest, GetPlayerByIdAndExceptionHandling) {
    GameState state;
    PlayerController controller(state);

    Player p1(1, "ply1");
    Player p2(2, "ply2");
    state.addPlayer(p1);
    state.addPlayer(p2);

    Player& found = controller.getPlayerById(2);
    EXPECT_EQ(found.name(), "ply2");

    found.setScore(500);
    EXPECT_EQ(state.mutablePlayers().at(1).score(), 500);

    EXPECT_THROW(controller.getPlayerById(999), std::runtime_error);
}

TEST(PlayerControllerTest, CreateAndAddPlayerEmitsSignalAndGeneratesUniqueIds) {
    GameState state;
    PlayerController controller(state);

    QSignalSpy spy(&controller, &PlayerController::playersUpdated);
    ASSERT_TRUE(spy.isValid());

    Player& p1 = controller.createAndAddPlayer("ply1");
    EXPECT_EQ(p1.name(), "ply1");
    EXPECT_EQ(state.players().size(), 1);
    EXPECT_EQ(spy.count(), 1);

    Player& p2 = controller.createAndAddPlayer("ply2");
    EXPECT_EQ(p2.name(), "ply2");
    EXPECT_GT(p2.id(), p1.id());
    EXPECT_EQ(state.players().size(), 2);
    EXPECT_EQ(spy.count(), 2);
}

TEST(PlayerControllerTest, AreAllGuessedHandlesGameLogic) {
    GameState state;
    PlayerController controller(state);

    state.setExplainerID(1);

    Player explainer(1, "Ведущий");
    Player p2(2, "Угадывающий 1");
    Player p3(3, "Угадывающий 2");

    state.addPlayer(explainer);
    state.addPlayer(p2);
    state.addPlayer(p3);

    EXPECT_FALSE(controller.areAllGuessed());


    state.mutablePlayers().at(1).setIsCurrentWordGuessed(true);
    EXPECT_FALSE(controller.areAllGuessed());

    state.mutablePlayers().at(2).setIsCurrentWordGuessed(true);
    
    EXPECT_TRUE(controller.areAllGuessed());
}

TEST(PlayerControllerTest, OnScoreUpdateEmitsPlayerScoreChangedSignal) {
    GameState state;
    PlayerController controller(state);

    QSignalSpy spy(&controller, &PlayerController::playerScoreChanged);
    ASSERT_TRUE(spy.isValid());

    int testPlayerId = 5;
    int testNewScore = 250;
    controller.onScoreUpdate(testPlayerId, testNewScore);

    ASSERT_EQ(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    EXPECT_EQ(arguments.at(0).toInt(), testPlayerId);
    EXPECT_EQ(arguments.at(1).toInt(), testNewScore);
}