#include "WordManager.hpp"


std::string WordManager::chooseRandomWord(){
    std::vector<std::string> words = getWordsFromFile("../russian.utf-8");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, words.size() - 1);

    int randomIndex = dis(gen);
    return words[randomIndex];
}

std::vector<std::string> WordManager::getWordsFromFile(std::string const fileName){
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

std::string WordManager::normalize(std::string str) {
    // trim left
    str.erase(
        str.begin(),
        std::find_if(str.begin(), str.end(),
            [](unsigned char ch) {
                return !std::isspace(ch);
            })
    );

    // trim right
    str.erase(
        std::find_if(str.rbegin(), str.rend(),
            [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(),
        str.end()
    );

    // to lowercase
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
        [](unsigned char ch) {
            return std::tolower(ch);
        }
    );

    return str;
}

void WordManager::openRandomLetter(GameState& state){
    if (state.countLettersToOpen() <= 0) return;  
    int numLetters = state.currentWord().length() / 2;
    if (numLetters == 0) return;

    bool hasClosed = false;
    for(const auto& s : state.openedLetters()) if(s == "_") { hasClosed = true; break; }
    if(!hasClosed) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, numLetters - 1);

    int randomIndex = dis(gen);
    

    while (state.openedLetters()[randomIndex] != "_") randomIndex = dis(gen); 

    std::string letter = state.currentWord().substr(randomIndex * 2, 2);
    std::vector<std::string> openedLetters_copy = state.openedLetters();
    openedLetters_copy[randomIndex] = letter;
    state.setOpenedLetters(openedLetters_copy);
    state.setCountLettersToOpen(state.countLettersToOpen()-1);
}

void WordManager::updateOpenedLetters(GameState& state){
    std::time_t curTime = std::time(nullptr);
    std::time_t roundEndTime = state.roundEndTime();
    const int ROUND_TIME = state.ROUND_TIME;
    if (curTime >= roundEndTime) return;
    std::time_t timePassed = (ROUND_TIME - (roundEndTime - curTime));
    int countLettersToOpenNow = timePassed/state.letterTimeInterval() - state.alreadyOpenedLetters();
    int countLetters = (countLettersToOpenNow > 0 ? countLettersToOpenNow : 0 );
    state.setAlreadyOpenedLetters(state.alreadyOpenedLetters() + countLettersToOpenNow);
    for (int i = countLettersToOpenNow; i>0; i--){
        openRandomLetter(state);
    }
}

void WordManager::setCurrentWord(GameState& state, std::string word, const int ROUND_TIME){
    state.setCurrentWord(normalize(word));
    state.setAlreadyOpenedLetters(0);
    int UTF8LettersPerSymbol = 2;
    double letterIntervalCoeff = 5/4.0;
    int numLetters = state.currentWord().length() / UTF8LettersPerSymbol;
    state.setCountLettersToOpen(numLetters / UTF8LettersPerSymbol + 1);
    state.setLetterTimeInterval(ROUND_TIME*letterIntervalCoeff/(state.countLettersToOpen()));
    state.openedLetters().clear();
    for (int i = 0; i < numLetters; ++i) {
        state.openedLetters().push_back("_");
    }
}

bool WordManager::isWordCorrect(GameState& state, const std::string word){
    return normalize(word) == state.currentWord();
}