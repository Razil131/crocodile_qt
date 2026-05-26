#include "RoundManager.hpp"
#include <QDebug>

void RoundManager::startNewRound(GameState& state){
    state.setRoundInProgress(true);
    if (state.RoundNum() == 0){
        state.setRoundCount(size(state.players()));
    }
    if (state.RoundCount() == state.RoundNum()){
        state.setRoundInProgress(false);
        return;
    }
    for (Player& ply:state.mutablePlayers()){
        ply.setIsCurrentWordGuessed(false);
    }
    state.setRoundNum(state.RoundNum() + 1);
    state.setRoundEndTime(std::time(nullptr)+state.ROUND_TIME);
}

void RoundManager::nextExplainer(GameState& state){
    bool flag = false;
    for (const Player& ply:state.players()){
        if (flag){
            state.setExplainerID(ply.id());
            return;
        }
        if (ply.id()==state.explainerID()){
            flag = true;
        }
    }
}

void RoundManager::restartGame(GameState& state){
    for (Player& ply:state.mutablePlayers()){
        ply.setScore(0);
    }
    state.setExplainerID(state.players()[0].id());
}