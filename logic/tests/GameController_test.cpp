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

//
// CHAT TESTS
//

TEST_F(GameControllerTest, ChatInitiallyEmpty)
{
    EXPECT_TRUE(controller.getChatHistory().empty());
}

TEST_F(GameControllerTest, SendSingleMessage)
{
    controller.sendMessage(p1, "hello");

    auto history = controller.getChatHistory();

    ASSERT_EQ(history.size(), 1);

    EXPECT_EQ(history[0].first, "Alice");
    EXPECT_EQ(history[0].second, "hello");
}

TEST_F(GameControllerTest, SendMultipleMessages)
{
    controller.sendMessage(p1, "hello");
    controller.sendMessage(p2, "world");
    controller.sendMessage(p3, "test");

    auto history = controller.getChatHistory();

    ASSERT_EQ(history.size(), 3);

    EXPECT_EQ(history[0].first, "Alice");
    EXPECT_EQ(history[1].first, "Bob");
    EXPECT_EQ(history[2].first, "Charlie");

    EXPECT_EQ(history[0].second, "hello");
    EXPECT_EQ(history[1].second, "world");
    EXPECT_EQ(history[2].second, "test");
}

TEST_F(GameControllerTest, ChatMessageOrderPreserved)
{
    controller.sendMessage(p1, "1");
    controller.sendMessage(p1, "2");
    controller.sendMessage(p1, "3");

    auto history = controller.getChatHistory();

    ASSERT_EQ(history.size(), 3);

    EXPECT_EQ(history[0].second, "1");
    EXPECT_EQ(history[1].second, "2");
    EXPECT_EQ(history[2].second, "3");
}

TEST_F(GameControllerTest, EmptyMessageCanBeSent)
{
    controller.sendMessage(p1, "");

    auto history = controller.getChatHistory();

    ASSERT_EQ(history.size(), 1);
    EXPECT_EQ(history[0].second, "");
}

TEST_F(GameControllerTest, LongMessageCanBeSent)
{
    std::string longMsg(10000, 'a');

    controller.sendMessage(p1, longMsg);

    auto history = controller.getChatHistory();

    ASSERT_EQ(history.size(), 1);
    EXPECT_EQ(history[0].second, longMsg);
}

//
// PLAYER TESTS
//

TEST_F(GameControllerTest, PlayersListInitiallyAccessible)
{
    auto players = controller.getPlayers();

    EXPECT_NO_THROW({
        auto p = controller.getPlayers();
    });
}

TEST_F(GameControllerTest, ReturnedPlayersVectorIndependent)
{
    auto players1 = controller.getPlayers();
    auto players2 = controller.getPlayers();

    EXPECT_EQ(players1.size(), players2.size());
}

//
// ROUND TIME TESTS
//

TEST_F(GameControllerTest, RoundTimePositive)
{
    EXPECT_GT(controller.getRoundTime(), 0);
}

TEST_F(GameControllerTest, TimeLeftNotNegativeImmediatelyAfterStart)
{
    controller.startRound();

    EXPECT_GE(controller.getTimeLeft(), 0);
}

TEST_F(GameControllerTest, TimeLeftLessOrEqualRoundTime)
{
    controller.startRound();

    EXPECT_LE(controller.getTimeLeft(),
              controller.getRoundTime());
}

//
// ROUND STATE TESTS
//

TEST_F(GameControllerTest, RoundNumberNonNegative)
{
    EXPECT_GE(controller.getRound(), 0);
}

TEST_F(GameControllerTest, RoundCountPositive)
{
    EXPECT_GT(controller.getRoundCount(), 0);
}

TEST_F(GameControllerTest, StartRoundDoesNotCrash)
{
    EXPECT_NO_THROW(controller.startRound());
}

TEST_F(GameControllerTest, MultipleRoundStartsDoNotCrash)
{
    EXPECT_NO_THROW({
        controller.startRound();
        controller.startRound();
        controller.startRound();
    });
}

//
// WORD TESTS
//

TEST_F(GameControllerTest, GetThreeWordsForChoose)
{
    auto [w1, w2, w3] = controller.getWordsForChoose();

    EXPECT_FALSE(w1.empty());
    EXPECT_FALSE(w2.empty());
    EXPECT_FALSE(w3.empty());
}

TEST_F(GameControllerTest, SetWordDoesNotCrash)
{
    EXPECT_NO_THROW(controller.setWord("apple"));
}

TEST_F(GameControllerTest, OpenedLettersAccessible)
{
    EXPECT_NO_THROW({
        auto letters = controller.getOpenedLetters();
    });
}


//
// EXPLAINER TESTS
//

TEST_F(GameControllerTest, isExplainerDoesNotCrash)
{
    EXPECT_NO_THROW(controller.isExplainer(p1));
}

//
// IP TESTS
//

TEST_F(GameControllerTest, IPStringAccessible)
{
    EXPECT_NO_THROW({
        auto ip = controller.getIP();
    });
}

TEST_F(GameControllerTest, IPNotEmpty)
{
    EXPECT_FALSE(controller.getIP().empty());
}

//
// STABILITY TESTS
//

TEST_F(GameControllerTest, SpamMessagesDoNotCrash)
{
    EXPECT_NO_THROW({
        for (int i = 0; i < 10000; ++i)
        {
            controller.sendMessage(p1, "spam");
        }
    });
}

TEST_F(GameControllerTest, ManyWordSelectionsDoNotCrash)
{
    EXPECT_NO_THROW({
        for (int i = 0; i < 1000; ++i)
        {
            controller.getWordsForChoose();
        }
    });
}

//
// EDGE CASES
//

TEST_F(GameControllerTest, UnicodeMessage)
{
    controller.sendMessage(p1, "привет");

    auto history = controller.getChatHistory();

    ASSERT_EQ(history.size(), 1);

    EXPECT_EQ(history[0].second, "привет");
}

TEST_F(GameControllerTest, EmojiMessage)
{
    controller.sendMessage(p1, "😀🔥🎮");

    auto history = controller.getChatHistory();

    ASSERT_EQ(history.size(), 1);

    EXPECT_EQ(history[0].second, "😀🔥🎮");
}

TEST_F(GameControllerTest, WordWithSpaces)
{
    EXPECT_NO_THROW(
        controller.setWord("hello world")
    );
}

TEST_F(GameControllerTest, WordWithUppercase)
{
    EXPECT_NO_THROW(
        controller.setWord("APPLE")
    );
}


//
// CONSISTENCY TESTS
//

TEST_F(GameControllerTest, ChatHistoryCallDoesNotModifyState)
{
    controller.sendMessage(p1, "hello");

    auto before = controller.getChatHistory();
    auto after = controller.getChatHistory();

    ASSERT_EQ(before.size(), after.size());

    EXPECT_EQ(before[0].second, after[0].second);
}

TEST_F(GameControllerTest, OpenedLettersCallDoesNotModifyState)
{
    controller.startRound();

    auto before = controller.getOpenedLetters();
    auto after = controller.getOpenedLetters();

    EXPECT_EQ(before.size(), after.size());
}

TEST_F(GameControllerTest, RoundGetterStable)
{
    auto r1 = controller.getRound();
    auto r2 = controller.getRound();

    EXPECT_EQ(r1, r2);
}

TEST_F(GameControllerTest, RoundCountGetterStable)
{
    auto r1 = controller.getRoundCount();
    auto r2 = controller.getRoundCount();

    EXPECT_EQ(r1, r2);
}