#include "GameController.hpp"
#include <QDebug>

GameController::GameController(){
    chatController_   = new ChatController(state_, wordmanager_);
    drawController_   = new DrawController();
    playerController_ = new PlayerController(state_);
    roundController_  = new RoundController(state_, wordmanager_, roundmanager_);
    networkManager_   = new NetworkManager(this);

    connect(playerController_, &PlayerController::playerAdded, this, &GameController::playerAdded);
    connect(playerController_, &PlayerController::playerScoreChanged, this, &GameController::playerScoreChanged);
    connect(chatController_, &ChatController::messageReceived, this, &GameController::messageReceived);
    connect(chatController_, &ChatController::chatUpdated, this, &GameController::chatUpdated);
    connect(playerController_, &PlayerController::playersUpdated, this, &GameController::playersUpdated);
    connect(chatController_, &ChatController::playersUpdated, this, &GameController::playersUpdated);
    
    connect(networkManager_, &NetworkManager::gameStateReceivedFromNetwork, this, [this](const GameState& newState) {
    qDebug() << "Client received state, wordChooseTimeLeft:" << newState.wordChooseTimeLeft();
    bool isNewRound = (this->state_.RoundNum() != newState.RoundNum());
    bool isChoosing = newState.isChoosingWord();
    
    this->state_ = newState;
    
    emit playersUpdated();
    emit chatUpdated();
    emit openedLettersUpdated(state_.openedLetters());
    emit timerUpdated(getTimeLeft()); 
    
    if (isChoosing) {
        emit wordTimerUpdated(state_.wordChooseTimeLeft());
        const auto& words = state_.wordsForChoose();
        if (words.size() == 3) {
            emit wordsForChooseReady(words[0], words[1], words[2]);
        }
    } else if (isNewRound) {
        emit roundStarted(state_.RoundNum(), state_.currentWord());
    }
});

    connect(networkManager_, &NetworkManager::clientIdAssigned, this, [this](int assignedId) {
        if (!state_.players().isEmpty()) {
            Player& localPlayer = state_.mutablePlayers().last();
            localPlayer.setID(assignedId);
            emit playersUpdated();
            emit localPlayerIdAssigned(assignedId);
        }
    });

    connect(networkManager_, &NetworkManager::serverDisconnected, this, &GameController::serverDisconnected);
}

void GameController::setupServerLogic() {
    connect(chatController_, &ChatController::messageReceived, this, [this](int senderId, const QString& senderName, const QString& text) {
        networkManager_->sendBroadcastMessage(senderName, text);
    });

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
        sendCurrentGameState();
    });

    connect(chatController_, &ChatController::openedLettersMayHaveChanged, 
            roundController_, &RoundController::onOpenedLettersUpdate);

    connect(roundController_, &RoundController::roundStarted, this, [this](int roundNum, const QString& wordToDraw) {
        emit roundStarted(roundNum, wordToDraw);
        sendCurrentGameState();
    });

    connect(roundController_, &RoundController::roundEnded, this, [this]() {
        emit this->roundEnded(); 
        QTimer::singleShot(0, this, [this]() {
            emit this->playersUpdated();
            sendCurrentGameState();
        });
    });

    connect(roundController_, &RoundController::timerUpdated, this, [this](std::time_t timeLeft) {
        emit timerUpdated(timeLeft);
    });

    connect(roundController_, &RoundController::openedLettersUpdated, this, [this](const QList<QString>& openedLetters) {
        emit openedLettersUpdated(openedLetters);
        sendCurrentGameState();
    });

    connect(roundController_, &RoundController::wordsForChooseReady, this, [this](const QString& w1, const QString& w2, const QString& w3) {
        emit wordsForChooseReady(w1, w2, w3);
        sendCurrentGameState(); 
    });

    connect(roundController_, &RoundController::explainerUpdated, this, &GameController::explainerUpdated);
    connect(roundController_, &RoundController::wordTimerUpdated, this, &GameController::wordTimerUpdated);
    connect(roundController_, &RoundController::wordsForChooseReady, this, &GameController::wordsForChooseReady);
    connect(roundController_, &RoundController::gameEnded, this, &GameController::gameEnded);

    connect(roundController_, &RoundController::wordTimerUpdated, this, [this](std::time_t timeLeft) {
        qDebug() << "Timer tick:" << timeLeft;
        emit wordTimerUpdated(timeLeft);
        sendCurrentGameState();
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

QString GameController::getIPandPort() {
    QString ip = networkManager_->getIP();
    quint16 port = getPort();
    QString result ="IP: " + ip + ":" + QString::number(port);
    qDebug() << port;
    return result;
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
    isServer_ = true;
    setupServerLogic(); 
    QString hostName = state_.players().isEmpty() ? "Host" : state_.players().first().name();
    networkManager_->startServer(port, hostName);
    networkManager_->setPort(port);
}

void GameController::connectToNetworkServer(const QString& ip, quint16 port, const QString& nickname) {
    isServer_ = false;
    playerController_->createAndAddPlayer(nickname);
    
    connect(networkManager_, &NetworkManager::connectionEstablished, this, &GameController::connectionSucceeded);
    connect(networkManager_, &NetworkManager::connectionFailed, this, &GameController::connectionFailed);

    networkManager_->connectToServer(ip, port);
    networkManager_->sendNickname(nickname);
}

void GameController::sendChatMessage(const QString& text) {
    if (isServer_) {
        if (!state_.players().isEmpty()) {
            chatController_->sendMessage(state_.mutablePlayers().first(), text);
        }
    } else {
        networkManager_->sendMessage(text);
    }
}

void GameController::selectWord(const QString& word) {
    if (isServer_) {
        roundController_->setWord(word);
    } else {
        networkManager_->sendSelectedWord(word);
    }
}

void GameController::startGame() {
    if (isServer_) {
        roundController_->startWordChooseAndRound();
    }
}

void GameController::sendDrawCommand(const DrawCommand& cmd) {
    emit drawCommandReceived(cmd);
    networkManager_->sendDraw(cmd);
}

void GameController::sendCurrentGameState() {
    if (isServer_) {
        networkManager_->sendState(state_);
    }
}

void GameController::processNetworkChatMessage(int senderId, const QString& text) {
    if (isServer_) {
        Player& player = playerController_->getPlayerById(senderId);
        chatController_->sendMessage(player, text); 
    }
}

void GameController::processNetworkWordSelection(int senderId, const QString& word) {
    if (isServer_) {
        if (senderId == state_.explainerID()) {
            roundController_->setWord(word);
        } else {
            qDebug() << "error word selection";
        }
    }
}

bool GameController::isServer(){
    return isServer_;
}

void GameController::shutdownNetwork() {
    if (networkManager_) {
        networkManager_->stopNetwork();
    }
}

void GameController::processPlayerDisconnect(int playerId) {
    if (!isServer_) return;

    auto& players = state_.mutablePlayers();
    for (auto it = players.begin(); it != players.end(); ++it) {
        if (it->id() == playerId) {
            players.erase(it);
            break;
        }
    }
    emit playersUpdated();
    sendCurrentGameState();
}