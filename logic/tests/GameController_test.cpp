#include <gtest/gtest.h>

#include "GameController.hpp"
#include "Player.hpp"

class GameControllerTest : public ::testing::Test {
protected:
    GameController controller;

    Player p1{"Alice"};
    Player p2{"Bob"};
    Player p3{"Charlie"};

    void SetUp() override {
        controller.addPlayer(p1);
        controller.addPlayer(p2);
        controller.addPlayer(p3);
        controller.setWord("adsadsadsa");
        controller.startRound();
    }
};



TEST_F(GameControllerTest, RoundNumberNonNegative)
{
    EXPECT_GE(controller.getRound(), 0);
}
