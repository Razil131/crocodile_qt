#include <gtest/gtest.h>
#include "ChatManager.hpp"
#include "GameState.hpp"
#include "WordManager.hpp"
#include "Player.hpp"

TEST(ChatManagerIntegration, AddRegularMessageReturnsFalseAndStoresIt) {
    GameState state;
    WordManager wordManager;
    ChatManager chatManager;

    Player player(2, "ply");
    wordManager.setCurrentWord(state, "Crocodile");

    bool isCorrectGuess = chatManager.addMessage(player, "42", state, wordManager);

    EXPECT_FALSE(isCorrectGuess);

    const auto& history = chatManager.messages();
    ASSERT_EQ(history.size(), 1);
    EXPECT_EQ(history[0].first, "ply");
    EXPECT_EQ(history[0].second, "42");
}

TEST(ChatManagerIntegration, CorrectGuessReturnsTrue) {
    GameState state;
    WordManager wordManager;
    ChatManager chatManager;

    Player explainer(1, "Explainer");
    state.addPlayer(explainer);

    Player player(2, "ply");
    state.addPlayer(player);
    wordManager.setCurrentWord(state, "Crocodile");

    bool isCorrectGuess = chatManager.addMessage(player, "Crocodile", state, wordManager);

    EXPECT_TRUE(isCorrectGuess);
}


TEST(ChatManagerIntegration, GuessIsCaseInsensitiveDueToNormalization) {
    GameState state;
    WordManager wordManager;
    ChatManager chatManager;


    Player explainer(1, "Explainer");
    state.addPlayer(explainer);

    Player player(3, "ply");
    state.addPlayer(player);
    wordManager.setCurrentWord(state, "Crocodile");

    bool isCorrectGuess = chatManager.addMessage(player, "CrOcOdIlE         ", state, wordManager);

    EXPECT_TRUE(isCorrectGuess);
}

TEST(ChatManagerIntegration, MaintainsMultipleMessagesInCorrectOrder) {
    GameState state;
    WordManager wordManager;
    ChatManager chatManager;

    Player p1(2, "ply1");
    Player p2(3, "ply2");
    wordManager.setCurrentWord(state, "Crocodile");

    chatManager.addMessage(p1, "42", state, wordManager);
    chatManager.addMessage(p2, "52", state, wordManager);
    chatManager.addMessage(p1, "67", state, wordManager);

    const auto& history = chatManager.messages();
    ASSERT_EQ(history.size(), 3);

    EXPECT_EQ(history[0].first, "ply1");
    EXPECT_EQ(history[0].second, "42");
    EXPECT_EQ(history[1].first, "ply2");
    EXPECT_EQ(history[1].second, "52");
}

TEST(ChatManagerIntegration, HandlesTrimmedSpacesInGuess) {
    GameState state;
    WordManager wordManager;
    ChatManager chatManager;


    Player explainer(1, "Explainer");
    state.addPlayer(explainer);

    Player player(4, "ply");
    state.addPlayer(player);
    wordManager.setCurrentWord(state, "Crocodile");

    bool isCorrectGuess = chatManager.addMessage(player, "  Crocodile  ", state, wordManager);

    EXPECT_TRUE(isCorrectGuess);
}