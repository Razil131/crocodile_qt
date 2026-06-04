#include "RoundController.hpp"

RoundController::RoundController(
    GameState& state,
    WordManager& wordManager,
    RoundManager& roundManager): state_(state),
    wordManager_(wordManager),
    roundManager_(roundManager){
    gameTimer_ = new QTimer(this);
    wordTimer_ = new QTimer(this);

    connect(
        gameTimer_,
        &QTimer::timeout,
        this,
        &RoundController::onGameTick
    );

    connect(
        wordTimer_,
        &QTimer::timeout,
        this,
        &RoundController::onWordTimerTick
    );
}


void RoundController::startWordChooseAndRound()
{
    wordTimeLeft_ = 10;
    wordChosen_ = false;
    currentWords_ = wordManager_.chooseWords();
    emit wordsForChooseReady(currentWords_[0],currentWords_[1],currentWords_[2]);
    if (!wordTimer_->isActive()){
        wordTimer_->start(1000);
    }   
}

void RoundController::setWord(const QString& word){
    if (wordChosen_) return;
    wordChosen_ = true;
    wordManager_.setCurrentWord(state_,word);
    wordTimer_->stop();
    roundManager_.startNewRound(state_);
    emit roundStarted(state_.RoundNum(), word);
    if (!gameTimer_->isActive()){
        gameTimer_->start(1000);
    }   
}

void RoundController::onGameTick()
{
    bool opened = wordManager_.updateOpenedLetters(state_);

    if (opened){
        emit openedLettersUpdated(state_.openedLetters());
    }
    std::time_t timeLeft = state_.roundEndTime() - std::time(nullptr);
    emit timerUpdated(timeLeft);
    
    if (timeLeft <= 0)
        stopRoundAndNext();

}

void RoundController::onWordTimerTick()
{
    emit wordTimerUpdated(static_cast<std::time_t>(--wordTimeLeft_));

    if (wordTimeLeft_ <= 0){
        wordTimer_->stop();
        if (!wordChosen_){
            setWord(currentWords_[0]);
        }  
    }
}

void RoundController::nextExplainer(){
    roundManager_.nextExplainer(state_);
    emit explainerUpdated(state_.explainerID());
};

void RoundController::onOpenedLettersUpdate(){
    emit openedLettersUpdated(state_.openedLetters());
}

void RoundController::stopRoundAndNext() 
{
    gameTimer_->stop();
    emit roundEnded();
    
    nextExplainer();
    if (state_.RoundNum()>=state_.RoundCount()){
        emit gameEnded();
        roundManager_.restartGame(state_);
        return;
    }
    
    QTimer::singleShot(1000, this, &RoundController::startWordChooseAndRound);
}