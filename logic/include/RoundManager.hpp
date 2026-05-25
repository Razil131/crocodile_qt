#pragma once

#include <vector>
#include "Player.hpp"
#include "GameState.hpp"
#include "ChatManager.hpp"
#include "exportMacro.hpp"

class CROCODILE_BACK_EXPORT RoundManager{
public:
    void startNewRound(GameState& state);
    void nextExplainer(GameState& state);

};