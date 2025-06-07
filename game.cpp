#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <windows.h>
#include <iomanip>
#import "game.h"

#define RED     12
#define GREEN   10
#define YELLOW  14
#define CYAN    11
#define MAGENTA 13
#define WHITE   15
#define GRAY    8

ColorManager color;

class GameWord {
public:
    std::string word;
    std::vector<bool> guessed_letters;

    GameWord(std::string w, const std::string& guessed = "") : word(w), guessed_letters(w.size(), false) {
        char first = tolower(w[0]);
        char last = tolower(w[w.size() - 1]);
        for (size_t i = 0; i < w.size(); ++i)
            if (tolower(w[i]) == first || tolower(w[i]) == last || guessed.find(tolower(w[i])) != std::string::npos)
                guessed_letters[i] = true;
    }

    std::string getDisplayWord() {
        std::string disp;
        for (size_t i = 0; i < word.length(); ++i)
            disp += guessed_letters[i] ? word[i] : '_';
        return disp;
    }

    bool guess(char c) {
        bool found = false;
        for (size_t i = 0; i < word.length(); ++i)
            if (tolower(word[i]) == tolower(c)) {
                guessed_letters[i] = true;
                found = true;
            }
        return found;
    }

    bool isComplete() {
        return std::all_of(guessed_letters.begin(), guessed_letters.end(), [](bool b){ return b; });
    }
};

std::vector<std::string> wordList;

void loadWords() {
    std::ifstream file("words.txt");
    std::string word;
    while (getline(file, word)) {
        if (!word.empty()) wordList.push_back(word);
    }
    if (wordList.empty()) {
        wordList = {"elephant", "banana", "canada", "hangman", "tiger"};
        color.print("Using default word list.\n", YELLOW);
    } else {
        color.print("Loaded " + std::to_string(wordList.size()) + " words.\n", CYAN);
    }
}

void saveGameState(const std::string& player, const std::string& word, const std::string& guessed, int attempts) {
    std::ofstream file("current_game.txt");
    file << player << "\n" << word << "\n" << guessed << "\n" << attempts << std::endl;
}

bool loadGameState(std::string& player, std::string& word, std::string& guessed, int& attempts) {
    std::ifstream file("current_game.txt");
    if (!file.is_open()) return false;
    getline(file, player);
    getline(file, word);
    getline(file, guessed);
    std::string attemptsStr;
    getline(file, attemptsStr);
    try {
        attempts = std::stoi(attemptsStr);
    } catch (...) {
        return false;
    }
    return !player.empty() && !word.empty();
}

void clearGameState() {
    std::ofstream("current_game.txt", std::ios::trunc);
}

void logHistory(const std::string& name, const std::string& word, int attempts, bool won) {
    std::ofstream file("history.txt", std::ios::app);
    file << name << " " << word << " " << attempts << " " << (won ? 1 : 0) << "\n";
}

void startGame(const std::string& player) {
    std::string word = wordList[rand() % wordList.size()];
    saveGameState(player, word, "", 5);

    std::cout << "\n";
    color.printLine('=', 50, CYAN);
    color.print("HANGMAN GAME STARTED\n", MAGENTA);
    color.printLine('=', 50, CYAN);

    color.print("Player: ", WHITE);
    color.print(player + "\n", GREEN);

    color.print("Word: ", WHITE);
    color.print(GameWord(word).getDisplayWord() + "\n", CYAN);

    color.print("Attempts remaining: ", WHITE);
    color.print("5\n", YELLOW);

    color.printLine('=', 50, CYAN);
    std::cout << "\n";
}

void guessLetter(char c) {
    std::string player, word, guessed;
    int attempts;
    if (!loadGameState(player, word, guessed, attempts)) {
        color.print(" No active game found.\n", RED);
        return;
    }

    if (guessed.find(tolower(c)) != std::string::npos) {
        color.print(" Letter '", YELLOW);
        color.print(std::string(1, c), WHITE);
        color.print("' already guessed.\n", YELLOW);
        return;
    }

    guessed += tolower(c);
    GameWord gw(word, guessed);
    bool correct = gw.guess(c);
    if (!correct) attempts--;

    std::cout << "\n";
    color.printLine('-', 40, GRAY);

    if (correct) {
        color.print("Good guess! Letter '", GREEN);
        color.print(std::string(1, c), WHITE);
        color.print("' found!\n", GREEN);
    } else {
        color.print(" Letter '", RED);
        color.print(std::string(1, c), WHITE);
        color.print("' not found.\n", RED);
    }

    color.print("Word: ", WHITE);
    color.print(gw.getDisplayWord() + "\n", CYAN);

    color.print("Attempts remaining: ", WHITE);
    if (attempts <= 1) {
        color.print(std::to_string(attempts) + "\n", RED);
    } else if (attempts <= 2) {
        color.print(std::to_string(attempts) + "\n", YELLOW);
    } else {
        color.print(std::to_string(attempts) + "\n", GREEN);
    }

    if (gw.isComplete()) {
        color.printLine('=', 40, GREEN);
        color.print(" CONGRATULATIONS! \n", GREEN);
        color.print("You guessed the word: ", WHITE);
        color.print(word + "\n", MAGENTA);
        color.print("Attempts used: ", WHITE);
        color.print(std::to_string(5 - attempts) + "/5\n", CYAN);
        color.printLine('=', 40, GREEN);

        std::vector<Player> players;
        std::ifstream in("leaderboard.txt");
        std::string name;
        int score, games;
        while (in >> name >> score >> games) {
            players.emplace_back(name, score, games);
        }
        in.close();

        bool found = false;
        for (auto& p : players) {
            if (p.name == player) {
                p.score += 1;
                p.games_played += 1;
                found = true;
                break;
            }
        }
        if (!found) {
            players.emplace_back(player, 1, 1);
        }

        std::ofstream out("leaderboard.txt");
        for (const auto& p : players) {
            out << p.name << " " << p.score << " " << p.games_played << "\n";
        }

        logHistory(player, word, 5 - attempts, true);
        clearGameState();
    }
    else if (attempts <= 0) {
        color.printLine('=', 40, RED);
        color.print("GAME OVER \n", RED);
        color.print("The word was: ", WHITE);
        color.print(word + "\n", MAGENTA);
        color.printLine('=', 40, RED);

        std::vector<Player> players;
        std::ifstream in("leaderboard.txt");
        std::string name;
        int score, games;
        while (in >> name >> score >> games) {
            players.emplace_back(name, score, games);
        }
        in.close();

        bool found = false;
        for (auto& p : players) {
            if (p.name == player) {
                p.games_played += 1;
                found = true;
                break;
            }
        }
        if (!found) {
            players.emplace_back(player, 0, 1);
        }

        std::ofstream out("leaderboard.txt");
        for (const auto& p : players) {
            out << p.name << " " << p.score << " " << p.games_played << "\n";
        }

        logHistory(player, word, 5, false);
        clearGameState();
    } else {
        saveGameState(player, word, guessed, attempts);
    }

    color.printLine('-', 40, GRAY);
    std::cout << "\n";
}

void showUsage() {
    std::cout << "\n";
    color.print("HANGMAN GAME\n", MAGENTA);
    color.printLine('=', 35, CYAN);

    color.print("Commands:\n", WHITE);
    std::cout << "  ";
    color.print("start_game <player>", CYAN);
    std::cout << "   - Start new game\n";

    std::cout << "  ";
    color.print("guess_letter <letter>", CYAN);
    std::cout << "  - Guess a letter\n";

    std::cout << "  ";
    color.print("view_word_status", CYAN);
    std::cout << "     - Show current word\n";

    std::cout << "  ";
    color.print("end_game", CYAN);
    std::cout << "            - End current game\n";

    color.printLine('=', 35, CYAN);
    std::cout << "\n";
}
void endGame() {
    std::string player, word, guessed;
    int attempts;
    if (loadGameState(player, word, guessed, attempts)) {
        color.print("Game ended for player: ", YELLOW);
        color.print(player + "\n", WHITE);
        logHistory(player, word, 5 - attempts, false);
    }

    clearGameState();
    color.print(" Game session ended.\n", YELLOW);
}

int main(int argc, char* argv[]) {
    srand(time(0));
    loadWords();

    if (argc < 2) {
        color.print(" Error: Missing command\n", RED);
        showUsage();
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "start_game" && argc == 3) {
        startGame(argv[2]);
    }
    else if (cmd == "guess_letter" && argc == 3) {
        guessLetter(argv[2][0]);
    }
    else if (cmd == "end_game") {
        endGame();
    }
    else {
        color.print("Unknown or incomplete command.\n", RED);
        showUsage();
    }

    return 0;
}
