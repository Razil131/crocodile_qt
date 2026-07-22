#include "WordManager.hpp"

QString WordManager::chooseRandomWord(){
    QList<std::string> rawWords = getWordsFromFile(":/assets/russian.utf-8");
    QList<QString> validWords;

    for (const auto& word : rawWords) {
        QString qWord = QString::fromStdString(word);
        int charCount = qWord.length();
        
        if (charCount >= 4 && charCount <= 12) {
            validWords.append(qWord);
        }
    }
    if (validWords.empty()) return "крокодил";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, validWords.size() - 1);

    int randomIndex = dis(gen);
    return validWords[randomIndex];
}

QList<std::string> WordManager::getWordsFromFile(const QString& fileName){
    QList<std::string> words;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "File does not exists" << fileName;
        return words; 
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            words.push_back(line.toStdString());
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
    QList<QString> openedLetters_copy = state.openedLetters();
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

QList<QString> WordManager::chooseWords(){
    QList<QString> words;
    words.clear();
    for (int i = 0; i<3; i++) // тк на выбор 3 слова
        words.push_back(chooseRandomWord());
    return words;
}