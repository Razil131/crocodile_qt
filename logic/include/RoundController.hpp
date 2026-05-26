#pragma once

#include <QObject>
#include <QTimer>
#include <vector>
#include <string>

#include "WordManager.hpp"
#include "RoundManager.hpp"

class CROCODILE_BACK_EXPORT RoundController: public QObject{
    Q_OBJECT
signals:
    void explainerUpdated(int newExplainerId);

    void wordTimerUpdated(std::time_t timeLeft);
    void roundEnded();
    void timerUpdated(std::time_t timeLeft);
    void openedLettersUpdated(const std::vector<QString>& openedLetters);
    void roundStarted(int roundNum, const QString& wordToDraw);
    void gameEnded();
    void wordsForChooseReady(const QString& w1, const QString& w2, const QString& w3);
private:
    GameState& state_;
    bool wordChosen_ = false;
    int wordTimeLeft_;
    std::vector<QString> currentWords_;
    QTimer* gameTimer_;
    QTimer* wordTimer_;
    RoundManager& roundManager_;
    WordManager& wordManager_;
    void onGameTick();
    void onWordTimerTick();
    void nextExplainer();

public:
    RoundController(GameState& state,
    WordManager& wordManager,
    RoundManager& roundManager);
    void startWordChooseAndRound();
    void setWord(const QString& word);

public slots:
    void onOpenedLettersUpdate();
    void stopRoundAndNext();
};