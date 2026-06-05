#include "WordManager.hpp"


QString WordManager::chooseRandomWord(){
    std::vector<std::string> words = getWordsFromFile("../russian.utf-8");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, words.size() - 1);

    int randomIndex = dis(gen);
    if (words.empty()) return "крокодил";
    return QString::fromStdString(words[randomIndex]);
}

std::vector<std::string> WordManager::getWordsFromFile(const std::string& fileName){
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "File didn't opened check the path!" << " " << fileName << std::endl;
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

QString WordManager::normalize(const QString& str) {
    return str.trimmed().toLower();
}

void WordManager::openRandomLetter(GameState& state){
    if (state.countLettersToOpen( ) <= 0) return;  
    int numLetters = state.currentWord().length();
    if (numLetters == 0) return;

    bool hasClosed = false;
    for(const auto& s : state.openedLetters()) if(s == "_") { hasClosed = true; break; }
    if(!hasClosed) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, numLetters - 1);

    int randomIndex = dis(gen);
    
    int attempts = 0;
    while (state.openedLetters()[randomIndex] != "_" and attempts < 100){
        randomIndex = dis(gen);
        attempts++;
    } 
    if (state.openedLetters()[randomIndex] != "_") return; 

    QString letter = state.currentWord().mid(randomIndex, 1);
    std::vector<QString> openedLetters_copy = state.openedLetters();
    openedLetters_copy[randomIndex] = letter;
    state.setOpenedLetters(openedLetters_copy);
    state.setCountLettersToOpen(state.countLettersToOpen()-1);
}

bool WordManager::updateOpenedLetters(GameState& state){
    std::time_t curTime = std::time(nullptr);
    std::time_t roundEndTime = state.roundEndTime();
    const int ROUND_TIME = state.ROUND_TIME;

    if (curTime >= roundEndTime) return false;

    double interval = state.letterTimeInterval();
    if (interval <= 0) return false;

    std::time_t timePassed = (ROUND_TIME - (roundEndTime - curTime));
    int countLettersToOpenNow = static_cast<int>(timePassed / interval) - state.alreadyOpenedLetters();

    int countLetters = (countLettersToOpenNow > 0 ? countLettersToOpenNow : 0 );
    if (countLetters == 0) return false;

    state.setAlreadyOpenedLetters(state.alreadyOpenedLetters() + countLetters);
    for (int i = countLetters; i>0; i--){
        openRandomLetter(state);
    }
    return true;
}

void WordManager::setCurrentWord(GameState& state, const QString& word){
    state.setCurrentWord(normalize(word));
    state.setAlreadyOpenedLetters(0);
    int numLetters = state.currentWord().length();
    int lettersToOpen = std::max(1, (numLetters / 2 + 1));
    state.setCountLettersToOpen(lettersToOpen);

    double letterIntervalCoeff = 4/5.0;
    double interval = (state.ROUND_TIME * letterIntervalCoeff) / lettersToOpen;
    state.setLetterTimeInterval(std::max(1.0, interval));

    state.clearAndResizeOpenedLetters(numLetters);
}

bool WordManager::isWordCorrect(GameState& state, const QString& word){
    return normalize(word) == state.currentWord();
}

std::vector<QString> WordManager::chooseWords(){
    std::vector<QString> words;
    words.clear();
    for (int i = 0; i<3; i++) // тк на выбор 3 слова
        words.push_back(chooseRandomWord());
    return words;
}