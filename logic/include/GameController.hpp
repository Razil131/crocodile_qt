#pragma once
#include <QObject>
#include <QTimer>

#include "GameState.hpp"
#include "ChatManager.hpp"
#include "WordManager.hpp"
#include "RoundManager.hpp"

class GameController: public QObject{
    Q_OBJECT
signals:
    void chatUpdated();
    void playersUpdated();
    void roundStarted();
    void roundEnded();
    void openedLettersUpdated();
    void timerUpdated(int secondsLeft);
    void explainerUpdated();
private:
    GameState state_;
    ChatManager chat_;
    WordManager wordmanager_;
    RoundManager roundmanager_;
    QTimer* gameTimer_;
    void onGameTick();
    bool canSendMessage(const Player& ply);
public:
    GameController();
    void sendMessage(Player& ply, std::string message); // когда игрок пишет сообщение 
    const std::vector<std::pair<std::string,std::string>>& getChatHistory(); // чтобы отображать историю чата возвращает вектор(<ник,сообщение>, <ник, сообщение>)
    const std::vector<Player>& getPlayers(); // чтобы отображать список игроков (посмотри че есть в классе Player)
    std::time_t getTimeLeft(); // чтобы отображать сколько осталось до конца раунда
    int getRoundTime(); // чтобы прогресс бар можно было настроить (максимальное значение)
    std::vector<std::string> getOpenedLetters(); // чтобы отображать слово снизу возвращает м _ ч в виде вектора строк
    std::string getIP(); // чтобы IP отображать
    int getRound(); // чтобы писать какой раунд сейчас
    int getRoundCount(); // чтобы писать сколько раундов всего 
    bool isExplainer(const Player& ply); // чтобы разрешить рисовать только ведущему
    std::tuple<std::string, std::string, std::string> getWordsForChoose(); // при нажатии на кнопку start получаешь и выводишь три слова полученные этой функцией
    void setWord(std::string word); // потом отправляешь выбранное слово сюда
    void startRound(); // и начинаешь раунд 
    void addPlayer(Player& ply); // добавь здесь игроков перед игрой
    void nextExplainer();
    std::string getWord();
};
