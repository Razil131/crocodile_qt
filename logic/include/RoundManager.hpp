#pragma once

#include <vector>
#include "Player.hpp"
#include "GameState.hpp"
#include "ChatManager.hpp"

class RoundManager{
public:
    void startNewRound(GameState& state);
    void nextExplainer(GameState& state);

};