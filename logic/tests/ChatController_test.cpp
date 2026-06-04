#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <tuple>
#include <string>
#include "ChatManager.hpp"
#include "Player.hpp"
#include "GameState.hpp"
#include "WordManager.hpp"

using ::testing::Return;
using ::testing::_;

class MockWordManager : public WordManager {
public:
    MOCK_METHOD(bool, isWordCorrect, (GameState& state, const QString& word), (override));
};

class ChatManagerTest : public ::testing::Test {
protected:
    ChatManager chatManager;
    GameState gameState;
    MockWordManager mockWordManager;
    
    Player ply1{6, "test_name1"};
    Player ply2{7, "test_name2"};
};


// ДОБАВЛЕНИЕ И ХРАНЕНИЕ СООБЩЕНИЙ

TEST_F(ChatManagerTest, InitialHistoryIsEmpty) {
    EXPECT_TRUE(chatManager.messages().empty());
}

TEST_F(ChatManagerTest, AddRegularMessageStoresInHistory) {
    // слово не угадано
    EXPECT_CALL(mockWordManager, isWordCorrect(_, QString("TEST")))
        .WillOnce(Return(false));

    bool isCorrect = chatManager.addMessage(ply1, "TEST", gameState, mockWordManager);
    
    EXPECT_FALSE(isCorrect);
    
    ASSERT_EQ(chatManager.messages().size(), 1);
    EXPECT_EQ(chatManager.messages()[0].first, "test_name1");
    EXPECT_EQ(chatManager.messages()[0].second, "TEST");
}

TEST_F(ChatManagerTest, AddMultipleMessagesStoresInCorrectOrder) {
    EXPECT_CALL(mockWordManager, isWordCorrect(_, _))
        .Times(2)
        .WillRepeatedly(Return(false));

    chatManager.addMessage(ply1, "First", gameState, mockWordManager);
    chatManager.addMessage(ply2, "Second", gameState, mockWordManager);

    ASSERT_EQ(chatManager.messages().size(), 2);
    EXPECT_EQ(chatManager.messages()[0].first, "test_name1");
    EXPECT_EQ(chatManager.messages()[0].second, "First");
    
    EXPECT_EQ(chatManager.messages()[1].first, "test_name2");
    EXPECT_EQ(chatManager.messages()[1].second, "Second");
}

// УГАДЫВАНИЕ СЛОВА


TEST_F(ChatManagerTest, CorrectGuessUpdatesPlayerAndGameState) {
    Player explainer(1, "Explainer");
    gameState.addPlayer(explainer);

    mockWordManager.setCurrentWord(gameState, "crocodile");

    EXPECT_CALL(mockWordManager, isWordCorrect(_, _))
        .WillOnce(Return(true));

    ASSERT_FALSE(ply1.isCurrentWordGuessed());
    int initialGuessedCount = ply1.guessedCount();

    bool isCorrect = chatManager.addMessage(ply1, "Crocodile", gameState, mockWordManager);

    EXPECT_TRUE(isCorrect);

    EXPECT_TRUE(ply1.isCurrentWordGuessed());
    EXPECT_EQ(ply1.guessedCount(), initialGuessedCount + 1);
}

TEST_F(ChatManagerTest, CorrectGuessChatHistoryBehavior) {
    Player explainer(1, "Ведущий");
    gameState.addPlayer(explainer);

    mockWordManager.setCurrentWord(gameState, "secretword");

    EXPECT_CALL(mockWordManager, isWordCorrect(_, _))
        .WillOnce(Return(true));

    bool isCorrect = chatManager.addMessage(ply1, "SecretWord", gameState, mockWordManager);
    
    EXPECT_TRUE(isCorrect);
    EXPECT_EQ(chatManager.messages().size(), 0);
}

TEST_F(ChatManagerTest, IncorrectGuessDoesNotMutatePlayer) {
    EXPECT_CALL(mockWordManager, isWordCorrect(_, QString("WrongWord")))
        .WillOnce(Return(false));

    bool isCorrect = chatManager.addMessage(ply1, "WrongWord", gameState, mockWordManager);

    EXPECT_FALSE(isCorrect);
    EXPECT_FALSE(ply1.isCurrentWordGuessed());
    EXPECT_EQ(ply1.guessedCount(), 0);
}


// ГРАНИЧНЫЕ И КРАЙНИЕ СОСТОЯНИЯ

TEST_F(ChatManagerTest, HandlesEmptyMessage) {
    EXPECT_CALL(mockWordManager, isWordCorrect(_, QString("")))
        .WillOnce(Return(false));

    bool isCorrect = chatManager.addMessage(ply1, "", gameState, mockWordManager);
    
    EXPECT_FALSE(isCorrect);
    ASSERT_EQ(chatManager.messages().size(), 1);
    EXPECT_EQ(chatManager.messages()[0].second, "");
}

TEST_F(ChatManagerTest, CaseSensitivityAndWhitespaceHandling) {
    QString rawMessage = "  Крокодил  ";
    
    EXPECT_CALL(mockWordManager, isWordCorrect(_, rawMessage))
        .WillOnce(Return(false));

    chatManager.addMessage(ply1, rawMessage, gameState, mockWordManager);
}