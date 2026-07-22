#pragma once

#include <QObject>
#include <QTimer>
#include <vector>
#include <string>
#include <QDebug>

#include "WordManager.hpp"
#include "RoundManager.hpp"

class CROCODILE_BACK_EXPORT RoundController: public QObject{
    Q_OBJECT
public:
    RoundController(GameState& state,
    WordManager& wordManager,
    RoundManager& roundManager);
    void startWordChooseAndRound();
    void setWord(const QString& word);

private:
    GameState& state_;
    bool wordChosen_ = false;
    int wordTimeLeft_;
    QList<QString> currentWords_;
    QTimer* gameTimer_;
    QTimer* wordTimer_;
    RoundManager& roundManager_;
    WordManager& wordManager_;
    void onGameTick();
    void onWordTimerTick();
    void nextExplainer();

public slots:
    void onOpenedLettersUpdate();
    void stopRoundAndNext();
    
signals:
    void explainerUpdated(int newExplainerId);

    void wordTimerUpdated(std::time_t timeLeft);
    void roundEnded();
    void timerUpdated(std::time_t timeLeft);
    void openedLettersUpdated(const QList<QString>& openedLetters);
    void roundStarted(int roundNum, const QString& wordToDraw);
    void gameEnded();
    void wordsForChooseReady(const QString& w1, const QString& w2, const QString& w3);

};