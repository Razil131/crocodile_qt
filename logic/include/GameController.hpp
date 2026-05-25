#pragma once
#include <QObject>
#include <QTimer>
#include "exportMacro.hpp"

#include "GameState.hpp"
#include "ChatManager.hpp"
#include "WordManager.hpp"
#include "RoundManager.hpp"
#include "drawCommand.hpp"



class CROCODILE_BACK_EXPORT GameController: public QObject{
    Q_OBJECT
signals:
    void chatUpdated(); // в чат чет написали +
    void playersUpdated(); // добавился игрок +
    void wordChooseStarted(); // начался выбор слова из трех (лишний)
    void roundStarted(); // начался раунд именно рисовать можно +
    void roundEnded(); // раунд закончился +
    void openedLettersUpdated(); // открылась новая буква +
    void timerUpdated(int secondsLeft); //+
    void explainerUpdated(); // поменялся ведущий +
    void wordsForChooseReady( // пора выбирать слово +
        const std::string& w1,
        const std::string& w2,
        const std::string& w3);
    void wordSelected(std::string word); // было выбрано слово (лишний)
    void wordTimerUpdated(int secondsLeft); //+
    void drawCommandReceived(DrawCommand cmd);

private:
    GameState state_;
    ChatManager chat_;
    WordManager wordmanager_;
    RoundManager roundmanager_;
    QTimer* gameTimer_;
    QTimer* wordTimer_;
    int wordTimeLeft_;
    bool wordChosen_ = false;
    std::vector<std::string> currentWords_;
    void onGameTick();
    bool canSendMessage(const Player& ply);
    void nextExplainer();
    std::time_t getTimeLeft();
    void onWordTimerTick();
public:
    GameController();
    void sendMessage(Player& ply, std::string message); // когда игрок пишет сообщение 
    const std::vector<std::pair<std::string,std::string>> getChatHistory(); // чтобы отображать историю чата возвращает вектор(<ник,сообщение>, <ник, сообщение>)
    const std::vector<Player>& getPlayers(); // чтобы отображать список игроков (посмотри че есть в классе Player)
    int getRoundTime(); // чтобы прогресс бар можно было настроить (максимальное значение)
    std::vector<std::string> getOpenedLetters(); // чтобы отображать слово снизу возвращает м _ ч в виде вектора строк
    std::string getIP(); // чтобы IP отображать
    int getRound(); // чтобы писать какой раунд сейчас
    int getRoundCount(); // чтобы писать сколько раундов всего 
    bool isExplainer(const Player& ply); // чтобы разрешить рисовать только ведущему
    void addPlayer(Player& ply); // добавь здесь игроков перед игрой
    void setWord(const std::string& word);
    std::vector<std::string> chooseWords();
    void startWordChooseAndRound();
    std::string getWord();

public slots:
    void broadcastCommand(DrawCommand cmd) {
        emit drawCommandReceived(cmd); 
    }

};
 // аргументы переделать в wordmanager.setword
 // не передается ведущий
 // не начинается следующий раунд 