#include "ChatController.hpp"


ChatController::ChatController(GameState& state, WordManager& wordManager):
    wordManager_(wordManager),
    state_(state){

    chat_ = ChatManager();
}

bool ChatController::canSendMessage(const Player& ply){
    if (state_.explainerID() == ply.id() or ply.isCurrentWordGuessed()){
            return false;
    }
    return true;
}


void ChatController::sendMessage(Player& ply, const QString& message){
    if (!canSendMessage(ply)) return;

    bool isCorrect = chat_.addMessage(ply, message, state_, wordManager_);
    
    if (isCorrect) {
        std::time_t timeLeft = state_.roundEndTime() - std::time(nullptr);
        int bonus = ply.calculateScoreForGuess(timeLeft, state_.ROUND_TIME);
        emit messageReceived(-1, "Система", QString("Игрок %1 угадал слово!").arg(ply.name()));
        emit openedLettersMayHaveChanged();
        emit playersUpdated();
        emit playerGuessedWord(ply.id(), bonus);
    } 
    else {
        emit messageReceived(ply.id(), ply.name(), message);
        emit chatUpdated();
    }
}