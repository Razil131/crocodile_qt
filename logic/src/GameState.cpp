#include "GameState.hpp"

GameState::GameState() :  
    roundEndTime_(std::time(nullptr) + ROUND_TIME),
    explainerID_(1),
    roundNum_(0),
    roundCount_(3),
    roundInProgress_(false),
    alreadyOpenedLetters_(0),
    countLettersToOpen_(0),
    letterTimeInterval_(0)
{}

Player& GameState::explainer() {
    for (Player& ply:players_){
        if (ply.id() == explainerID_){
            return ply;
        }
    }
    throw std::runtime_error("There is no player with explainer index: " + std::to_string(explainerID_));
}

void GameState::clearAndResizeOpenedLetters(int size) {
    openedLetters_.clear();
    openedLetters_.resize(size, "_");
}

// вывод
QDataStream &operator<<(QDataStream &out, const GameState &state) {
    out << qint64(state.roundEndTime())
        << state.players()              
        << qint32(state.explainerID())
        << qint32(state.RoundNum())
        << qint32(state.RoundCount())
        << state.isRoundInProgress()
        << state.currentWord()
        << state.openedLetters()   
        << qint32(state.alreadyOpenedLetters())
        << qint32(state.countLettersToOpen())
        << qint32(state.letterTimeInterval())
        << state.isChoosingWord()
        << state.wordsForChoose()
        << qint32(state.wordChooseTimeLeft())
        << state.isGameEnded();
        
    return out;
}

// ввод
QDataStream &operator>>(QDataStream &in, GameState &state) {
    qint64 roundEndTime;
    QList<Player> players;
    qint32 explainerID;
    qint32 roundNum;
    qint32 roundCount;
    bool roundInProgress;
    QString currentWord;
    QList<QString> openedLetters;
    qint32 alreadyOpenedLetters;
    qint32 countLettersToOpen;
    qint32 letterTimeInterval;
    bool isChoosingWord;
    QList<QString> wordsForChoose;
    qint32 wordChooseTimeLeft;
    bool gameEnded;
    

    in >> roundEndTime 
       >> players 
       >> explainerID 
       >> roundNum 
       >> roundCount 
       >> roundInProgress 
       >> currentWord 
       >> openedLetters 
       >> alreadyOpenedLetters 
       >> countLettersToOpen 
       >> letterTimeInterval
       >> isChoosingWord
       >> wordsForChoose
       >> wordChooseTimeLeft
       >> gameEnded;

    state.setRoundEndTime(static_cast<std::time_t>(roundEndTime));
    state.mutablePlayers() = players;
    state.setExplainerID(explainerID);
    state.setRoundNum(roundNum);
    state.setRoundCount(roundCount);
    state.setRoundInProgress(roundInProgress);
    state.setCurrentWord(currentWord);
    state.setOpenedLetters(openedLetters);
    state.setAlreadyOpenedLetters(alreadyOpenedLetters);
    state.setCountLettersToOpen(countLettersToOpen);
    state.setLetterTimeInterval(letterTimeInterval);
    state.setIsChoosingWord(isChoosingWord);
    state.setWordsForChoose(wordsForChoose);
    state.setWordChooseTimeLeft(wordChooseTimeLeft);
    state.setGameEnded(gameEnded);

    return in;
}
