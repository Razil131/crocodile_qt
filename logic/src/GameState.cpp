#include "GameState.hpp"


std::vector<std::string> GameState::getWordsFromFile(std::string const fileName){
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл!" << std::endl;
    }

    std::vector<std::string> words;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            words.push_back(line);
        }
    }

    file.close();
    return words;
}

void GameState::openRandomLetter(){
    if (countLettersToOpen_ <= 0) return;  
    int numLetters = currentWord_.length() / 2;
    if (numLetters == 0) return;

    bool hasClosed = false;
    for(const auto& s : openedLetters_) if(s == "_") { hasClosed = true; break; }
    if(!hasClosed) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, numLetters - 1);

    int randomIndex = dis(gen);
    

    while (openedLetters_[randomIndex] != "_") randomIndex = dis(gen); 

    std::string letter = currentWord_.substr(randomIndex * 2, 2);
    openedLetters_[randomIndex] = letter;
    countLettersToOpen_--;
}

void GameState::updateOpenedLetters(){
    std::time_t curTime = std::time(nullptr);
    if (curTime >= roundEndTime_) return;
    std::time_t timePassed = (ROUND_TIME - (roundEndTime_ - curTime));
    int countLettersToOpenNow = timePassed/letterTimeInterval_ - alreadyOpenedLetters_;
    int countLetters = (countLettersToOpenNow > 0 ? countLettersToOpenNow : 0 );
    alreadyOpenedLetters_ += countLettersToOpenNow;
    for (int i = countLettersToOpenNow; i>0; i--){
        openRandomLetter();
    }
}

std::string GameState::chooseRandomWord(){
    std::vector<std::string> words = getWordsFromFile("../russian_easy.utf-8");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, words.size() - 1);

    int randomIndex = dis(gen);
    return words[randomIndex];
}

bool GameState::isWordCorrect(const std::string word) const{
    return word == currentWord_;
}

GameState::GameState() : 
    currentWord_(chooseRandomWord()),
    alreadyOpenedLetters_(0), 
    roundEndTime_(std::time(nullptr) + ROUND_TIME)
{
    int UTF8LettersPerSymbol = 2;
    double letterIntervalCoeff = 5/4.0;
    int numLetters = currentWord_.length() / UTF8LettersPerSymbol;
    countLettersToOpen_ = numLetters / 2 + 1;
    letterTimeInterval_ = ROUND_TIME*letterIntervalCoeff/(countLettersToOpen_);
    for (int i = 0; i < numLetters; ++i) {
        openedLetters_.push_back("_");
    }

    for (const auto& s : openedLetters_) std::cout << s; // DEBUG
    std::cout << " " << currentWord_ << " " << countLettersToOpen_ << "\n";
    openRandomLetter();
    for (const auto& s : openedLetters_) std::cout << s;
    std::cout << " " << currentWord_ << " " << countLettersToOpen_ << "\n";
    openRandomLetter();

}


