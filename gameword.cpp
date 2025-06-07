#include "gameword.h"
#include <cctype>
#include <algorithm>

GameWord::GameWord(std::string w, const std::string& guessed)
    : word(w), guessed_letters(w.size(), false)
{
    char first = tolower(w[0]);
    char last = tolower(w[w.size() - 1]);

    for (size_t i = 0; i < w.size(); ++i) {
        if (tolower(w[i]) == first || tolower(w[i]) == last || guessed.find(tolower(w[i])) != std::string::npos)
            guessed_letters[i] = true;
    }
}

std::string GameWord::getDisplayWord() {
    std::string disp;
    for (size_t i = 0; i < word.length(); ++i) {
        disp += guessed_letters[i] ? word[i] : '_';
    }
    return disp;
}

bool GameWord::guess(char c) {
    bool found = false;
    for (size_t i = 0; i < word.length(); ++i) {
        if (tolower(word[i]) == tolower(c)) {
            guessed_letters[i] = true;
            found = true;
        }
    }
    return found;
}

bool GameWord::isComplete() {
    return std::all_of(guessed_letters.begin(), guessed_letters.end(), [](bool b) { return b; });
}
