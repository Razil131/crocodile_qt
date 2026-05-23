#include "GameController.hpp"

GameController::GameController(){
    chat_ = ChatManager();
    state_ = GameState();
    wordmanager_ = WordManager();
    roundmanager_ = RoundManager();
    gameTimer_ = new QTimer(this);

    connect(
        gameTimer_,
        &QTimer::timeout,
        this,
        &GameController::onGameTick
    );

    gameTimer_->start(1000);
}

bool GameController::canSendMessage(const Player& ply){
    if (!isExplainer(ply)){
        if (!ply.isCurrentWordGuessed()){
            return true;
        }
    }
    return false;
}

void GameController::onGameTick()
{
    bool opened = wordmanager_.updateOpenedLetters(state_);

    if (opened)
        emit openedLettersUpdated();

    auto timeLeft = getTimeLeft();

    emit timerUpdated(timeLeft);

    if (timeLeft <= 0)
    {
        emit roundEnded();
        gameTimer_->stop();
    }
}

void GameController::sendMessage(Player& ply, std::string message){
    if (canSendMessage(ply)){
        chat_.addMessage(ply,message,state_,wordmanager_);
        emit chatUpdated();
    }
}

const std::vector<std::pair<std::string,std::string>>& GameController::getChatHistory(){
    return chat_.messages();
}

const std::vector<Player>& GameController::getPlayers(){
    return state_.players();
}

std::time_t GameController::getTimeLeft(){
    std::time_t time_left = state_.roundEndTime()-std::time(nullptr);
    std::cout << time_left;
    return time_left >= 0 ? time_left : 0;
}

int GameController::getRoundTime(){
    return state_.ROUND_TIME;
}

std::vector<std::string> GameController::getOpenedLetters(){
    return state_.openedLetters();
}

std::string GameController::getIP(){
    return "255.255.255.255:65535"; // TODO
}

int GameController::getRound(){
    return state_.RoundNum();
}

int GameController::getRoundCount(){
    return state_.RoundCount();
}

bool GameController::isExplainer(const Player& ply){
    return ply.id() == state_.explainerID();
}

void GameController::setWord(std::string word){
    wordmanager_.setCurrentWord(state_, word, state_.ROUND_TIME);
}

std::tuple<std::string, std::string, std::string> GameController::getWordsForChoose(){
    return std::make_tuple(wordmanager_.chooseRandomWord(), wordmanager_.chooseRandomWord(), wordmanager_.chooseRandomWord());
}

void GameController::startRound(){
    if (getWord()== ""){return;}
    if (getTimeLeft() >= 0){return;}
    roundmanager_.startNewRound(state_);
    if (!gameTimer_->isActive())
        gameTimer_->start(1000);
    emit roundStarted();
}

void GameController::addPlayer(Player& ply){
    state_.addPlayer(ply);
    emit playersUpdated();
}

std::string GameController::getWord(){
    return state_.currentWord();
}

void GameController::nextExplainer(){
    roundmanager_.nextExplainer(state_);
    emit explainerUpdated();
};
