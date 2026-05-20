#include "ChatManager.hpp"

void ChatManager::addMessage(Player& ply, std::string& message, GameState& gameState, WordManager& wordManager){
    tryGuess(ply,gameState,wordManager,message);
    messages_.emplace_back(ply.name(), message);
}

bool ChatManager::tryGuess(Player& ply, GameState& gameState, WordManager& wordManager, const std::string& word){
    if (ply.id() == gameState.explainerID()) return false;
    bool isCorrect = wordManager.isWordCorrect(gameState,word);
    if (isCorrect){
        ply.addOneToGuessedCount();
        std::time_t curTime = std::time(nullptr);
        ply.setIsCurrentWordGuessed(true);
        std::time_t timeLeftTillRoundEnd = gameState.roundEndTime() - curTime;
        ply.addScore(ply.calculateScoreForGuess(timeLeftTillRoundEnd,gameState.ROUND_TIME));
        int scoreForGoodDrawing = gameState.explainer().calculateScoreForDrawing(timeLeftTillRoundEnd,gameState.ROUND_TIME);
        gameState.explainer().addScore(scoreForGoodDrawing);
    }
    return isCorrect;
}