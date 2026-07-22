#include "ChatManager.hpp"

bool ChatManager::addMessage(Player& ply, const QString& message, GameState& gameState, WordManager& wordManager){
    bool isCorrect = tryGuess(ply,gameState,wordManager,message);
    if (!isCorrect){
        messages_.emplace_back(ply.name(), message);
    }
    return isCorrect;
}

bool ChatManager::tryGuess(Player& ply, GameState& gameState, WordManager& wordManager, const QString& word){
    if (ply.id() == gameState.explainerID()) {
        return false;
    }
    bool isCorrect = wordManager.isWordCorrect(gameState,word);
    if (isCorrect){
        ply.addOneToGuessedCount();
        std::time_t curTime = std::time(nullptr);
        ply.setIsCurrentWordGuessed(true);
        std::time_t timeLeftTillRoundEnd = gameState.roundEndTime() - curTime;
        std::cout << "score: " << ply.calculateScoreForGuess(timeLeftTillRoundEnd,gameState.ROUND_TIME);
        ply.addScore(ply.calculateScoreForGuess(timeLeftTillRoundEnd,gameState.ROUND_TIME));
        int scoreForGoodDrawing = gameState.explainer().calculateScoreForDrawing(timeLeftTillRoundEnd,gameState.ROUND_TIME);
        gameState.explainer().addScore(scoreForGoodDrawing);
    }
    return isCorrect;
}