#include "RoundManager.hpp"
#include <QDebug>

void RoundManager::startNewRound(GameState& state){
    state.setRoundInProgress(true);
    if (state.RoundNum() == 0){
        state.setRoundCount(state.players().size());
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

void RoundManager::nextExplainer(GameState& state) {
    const auto& players = state.players();
    if (players.empty()) return;

    size_t currentIdx = 0;
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i].id() == state.explainerID()) {
            currentIdx = i;
            break;
        }
    }

    size_t nextIdx = (currentIdx + 1) % players.size();
    state.setExplainerID(players[nextIdx].id());
}

void RoundManager::restartGame(GameState& state){
    for (Player& ply:state.mutablePlayers()){
        ply.setScore(0);
    }
    state.setRoundNum(0);
    state.setExplainerID(state.players()[0].id());
}