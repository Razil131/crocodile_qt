#include "GameController.hpp"
#include <QDebug>

GameController::GameController(){

    chatController_   = new ChatController(state_, wordmanager_);
    drawController_   = new DrawController();
    playerController_ = new PlayerController(state_);
    roundController_  = new RoundController(state_, wordmanager_, roundmanager_);
    networkManager_ = new NetworkManager(this);

    connect(chatController_, &ChatController::playerGuessedWord, this, [this](int playerId, int scoreBonus) {
        Player& ply = playerController_->getPlayerById(playerId);
        
        int newPlayerScore = ply.score() + scoreBonus; 
        playerController_->onScoreUpdate(ply.id(), newPlayerScore);
        
        Player& explainer = state_.explainer();
        int newExplainerScore = explainer.score() + scoreBonus;
        playerController_->onScoreUpdate(explainer.id(), newExplainerScore);

        if (playerController_->areAllGuessed()) {
            roundController_->stopRoundAndNext();
        }
    });

    connect(chatController_, &ChatController::openedLettersMayHaveChanged, 
            roundController_, &RoundController::onOpenedLettersUpdate);


    connect(playerController_, &PlayerController::playerAdded, this, &GameController::playerAdded);
    connect(playerController_, &PlayerController::playerScoreChanged, this, &GameController::playerScoreChanged);
    connect(playerController_, &PlayerController::playersUpdated, this, &GameController::playersUpdated);

    connect(chatController_, &ChatController::messageReceived, this, &GameController::messageReceived);
    connect(chatController_, &ChatController::chatUpdated, this, &GameController::chatUpdated);

    connect(drawController_, &DrawController::drawCommandReceived, this, &GameController::drawCommandReceived);
    
    connect(playerController_, &PlayerController::playersUpdated, this, &GameController::playersUpdated);
    connect(chatController_, &ChatController::playersUpdated, this, &GameController::playersUpdated);
    connect(roundController_, &RoundController::explainerUpdated, this, &GameController::explainerUpdated);
    connect(roundController_, &RoundController::wordTimerUpdated, this, &GameController::wordTimerUpdated);
    connect(roundController_, &RoundController::roundStarted, this, &GameController::roundStarted);
    connect(roundController_, &RoundController::roundEnded, this, [this]() {
        emit this->roundEnded(); 

        QTimer::singleShot(0, this, [this]() {
            emit this->playersUpdated();
        });
    });
    connect(roundController_, &RoundController::timerUpdated, this, &GameController::timerUpdated);
    connect(roundController_, &RoundController::openedLettersUpdated, this, &GameController::openedLettersUpdated);
    connect(roundController_, &RoundController::wordsForChooseReady, this, &GameController::wordsForChooseReady);
    connect(roundController_, &RoundController::gameEnded, this, &GameController::gameEnded);
    connect(networkManager_, &NetworkManager::gameStateReceived, this, [this](const GameState& newState) {
        this->state_ = newState;
        emit playersUpdated();
        emit chatUpdated();
    });
}

const QList<std::pair<QString, QString>>& GameController::getChatHistory() const {
    static QList<std::pair<QString, QString>> dummy;
    return dummy; 
}

const QList<Player>& GameController::getPlayers() {
    return state_.players();
}

int GameController::getRoundTime() {
    return state_.ROUND_TIME;
}

QList<QString> GameController::getOpenedLetters() {
    return state_.openedLetters();
}

QString GameController::getIP() {
    return "255.255.255.255:65535"; // TODO: Сюда можно будет прикрутить реальный адрес QHostAddress
}

int GameController::getRound() {
    return state_.RoundNum();
}

int GameController::getRoundCount() {
    return state_.RoundCount();
}

QString GameController::getWord() {
    return state_.currentWord();
}

std::time_t GameController::getTimeLeft() {
    std::time_t time_left = state_.roundEndTime() - std::time(nullptr);
    return time_left >= 0 ? time_left : 0;
}

// NETWORK

void GameController::startNetworkServer(quint16 port) {
    networkManager_->startServer(port);
}

void GameController::connectToNetworkServer(const QString& ip, quint16 port) {
    networkManager_->connectToServer(ip, port);
}

void GameController::sendChatMessage(const QString& text) {

    networkManager_->sendMessage(text);
}

void GameController::sendDrawCommand(const DrawCommand& cmd) {
    networkManager_->sendDraw(cmd);
}

void GameController::sendCurrentGameState() {
    networkManager_->sendState(state_);
}