#include <iostream>
#include <windows.h>
#include "GameState.hpp"
#include "Player.hpp"

int main(int, char**){
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    GameState gs;
    Player a("adsadas");
    std::string ab = "adsadsada";
    while (!a.tryToGuessWord(gs,ab)){
        std::cin >> ab;
        for (const auto& s : gs.openedLetters()) std::cout << s;
    }
    std::cout << "\n" << a.getScore() << " " << a.getId() << " " << a.getName() << " " << a.guessedCount() << " ";
}
