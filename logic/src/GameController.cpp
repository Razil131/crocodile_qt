#include "GameController.hpp"
#include <QDebug>
GameController::GameController(){
    qDebug() << "!!!!!!!!!!!!!";
    chat_ = ChatManager();
    state_ = GameState();
    wordmanager_ = WordManager();
    roundmanager_ = RoundManager();
    gameTimer_ = new QTimer(this);
    wordTimer_ = new QTimer(this);

    connect(
        gameTimer_,
        &QTimer::timeout,
        this,
        &GameController::onGameTick
    );

    connect(
        wordTimer_,
        &QTimer::timeout,
        this,
        &GameController::onWordTimerTick
    );
}

bool GameController::canSendMessage(const Player& ply){
    if (!isExplainer(ply)){
        if (!ply.isCurrentWordGuessed()){
            return true;
        }
    }
    return false;
}


void GameController::sendMessage(Player& ply, std::string message){
    if (canSendMessage(ply)){
        chat_.addMessage(ply,message,state_,wordmanager_);
        emit chatUpdated();
    }
}

const std::vector<std::pair<std::string,std::string>> GameController::getChatHistory(){
    return chat_.messages();
}

const std::vector<Player>& GameController::getPlayers(){
    return state_.players();
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


void GameController::addPlayer(Player& ply){
    state_.addPlayer(ply);
    emit playersUpdated();
}

std::time_t GameController::getTimeLeft(){
    std::time_t time_left = state_.roundEndTime()-std::time(nullptr);
    return time_left >= 0 ? time_left : 0;
}



void GameController::nextExplainer(){
    roundmanager_.nextExplainer(state_);
    emit explainerUpdated();
};

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
        nextExplainer();

        QTimer::singleShot(0, this, &GameController::startWordChooseAndRound);
    }
}

void GameController::setWord(const std::string& word){
    if (wordChosen_) return;
    wordChosen_ = true;
    wordmanager_.setCurrentWord(state_,word,getRoundTime());
    emit wordSelected(word);
    wordTimer_->stop();
    roundmanager_.startNewRound(state_);
    emit roundStarted();
    if (!gameTimer_->isActive()){
        gameTimer_->start(1000);
    }   
}

std::vector<std::string> GameController::chooseWords(){
    std::vector<std::string> words;
    words.clear();
    for (int i = 0; i<3; i++) // тк на выбор 3 слова
        words.push_back(wordmanager_.chooseRandomWord());
    return words;
}

void GameController::onWordTimerTick()
{
    emit wordTimerUpdated(--wordTimeLeft_);

    if (wordTimeLeft_ <= 0){
        wordTimer_->stop();
        if (!wordChosen_){
            setWord(currentWords_[0]);
        }  
    }
}

void GameController::startWordChooseAndRound()
{
    emit wordChooseStarted();
    wordTimeLeft_ = 10;
    wordChosen_ = false;
    currentWords_ = chooseWords();
    emit wordsForChooseReady(currentWords_[0],currentWords_[1],currentWords_[2]);
    if (!wordTimer_->isActive()){
        wordTimer_->start(1000);
    }   
}

std::string GameController::getWord(){
    return state_.currentWord();
}
