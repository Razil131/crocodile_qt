#pragma once
#include <stdexcept>
#include <QObject>
#include "Player.hpp"
#include "GameState.hpp"

class CROCODILE_BACK_EXPORT PlayerController: public QObject{
    Q_OBJECT
private:
    GameState& state_;
    int nextPlayerId_ = 1;

public:
    PlayerController(GameState& state);
    Player& createAndAddPlayer(const QString& name);
    Player& getPlayerById(int id);
    bool isExplainer(const Player& ply);
    bool isExplainerByID(const int id);
    bool areAllGuessed();

public slots:
    void onScoreUpdate(int playerId, int newScore);

signals:
    void playerAdded(int playerId, const QString& name);
    void playerScoreChanged(int playerId, int newScore);
    void playersUpdated();

};