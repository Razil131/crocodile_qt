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
    state.setRoundEndTime(std::time(nullptr)+state.ROUND_TIME);
    if (state.RoundNum() != 0){
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
    }
}