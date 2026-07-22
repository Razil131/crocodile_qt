#include "PlayerController.hpp"

PlayerController::PlayerController(GameState& state):
    state_(state){}

Player& PlayerController::createAndAddPlayer(const QString& name){
    int assignedId = nextPlayerId_++;
    
    Player newPlayer(assignedId, name);
    
    state_.addPlayer(newPlayer);
    
    emit playersUpdated();
    
    return state_.mutablePlayers().back();
}

Player& PlayerController::getPlayerById(int id){
    for (Player& p : state_.mutablePlayers()){
        if (p.id() == id)
            return p;
    }

    throw std::runtime_error("player not found");
}

bool PlayerController::isExplainer(const Player& ply){
    return ply.id() == state_.explainerID();
}

bool PlayerController::isExplainerByID(const int id){
    return id == state_.explainerID();
}

bool PlayerController::areAllGuessed(){
    for (const Player& ply : state_.players()){
        if (!ply.isCurrentWordGuessed() && !isExplainer(ply)){
            return false;
        }
    }
    return true;
}

void PlayerController::onScoreUpdate(int playerId, int newScore)
{
    emit playerScoreChanged(playerId, newScore);
}