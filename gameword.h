#ifndef GAMEWORD_HPP
#define GAMEWORD_HPP

#include <string>
#include <vector>

class GameWord {
private:
    std::string word;
    std::vector<bool> guessed_letters;

public:
    GameWord(std::string w, const std::string& guessed = "");

    std::string getDisplayWord();
    bool guess(char c);
    bool isComplete();
};

#endif
