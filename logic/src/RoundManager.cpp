#include "RoundManager.hpp"


void RoundManager::startNewRound(GameState& state){
    state.setRoundInProgress(true);
    if (state.RoundNum() == 0){
        state.setRoundCount(size(state.players()));
    }
    if (state.RoundCount() == state.RoundNum()){
        return;
    }
    state.setRoundNum(state.RoundNum() + 1);

    bool flag = false;
    for (const Player ply:state.players()){
        if (ply.id()==state.explainerID()){
            flag = true;
        }
        if (flag){
            state.setExplainerID(ply.id());
            return;
        }
    }

    state.setRoundEndTime(std::time_t(nullptr)+state.ROUND_TIME);
}