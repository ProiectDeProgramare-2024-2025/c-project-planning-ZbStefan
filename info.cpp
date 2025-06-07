#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <windows.h>
#include <iomanip>
#include "info.h"

#define RED     12
#define GREEN   10
#define YELLOW  14
#define CYAN    11
#define MAGENTA 13
#define WHITE   15
#define GRAY    8

class ColorManager {
private:
    HANDLE hConsole;
    WORD originalColor;

public:
    ColorManager() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(hConsole, &info);
        originalColor = info.wAttributes;
    }

    void setColor(int color) {
        SetConsoleTextAttribute(hConsole, color);
    }

    void resetColor() {
        SetConsoleTextAttribute(hConsole, originalColor);
    }

    void print(const std::string& text, int color) {
        setColor(color);
        std::cout << text;
        resetColor();
    }

    void printLine(char c = '-', int length = 40, int color = GRAY) {
        setColor(color);
        std::cout << std::string(length, c) << "\n";
        resetColor();
    }
};

ColorManager color;

struct Stats {
    int wins = 0;
    int losses = 0;
};

void displayLeaderboardFromHistory() {
    std::ifstream file("history.txt");
    if (!file.is_open()) {
        color.print("No history file found.\n", YELLOW);
        return;
    }

    std::unordered_map<std::string, Stats> stats;
    std::string name, word;
    int attempts, result;

    while (file >> name >> word >> attempts >> result) {
        if (result == 1)
            stats[name].wins++;
        else
            stats[name].losses++;
    }

    if (stats.empty()) {
        color.print("No completed games found.\n", YELLOW);
        return;
    }

    std::vector<std::pair<std::string, Stats>> sortedPlayers(stats.begin(), stats.end());
    std::sort(sortedPlayers.begin(), sortedPlayers.end(), [](const auto& a, const auto& b) {
        return a.second.wins > b.second.wins;
    });

    std::cout << "\n";
    color.print("LEADERBOARD (History-Based)\n", MAGENTA);
    color.printLine('=', 40, CYAN);

    color.setColor(WHITE);
    std::cout << std::left << std::setw(5) << "Rank"
              << std::setw(15) << "Player"
              << std::setw(10) << "Wins"
              << std::setw(10) << "Losses" << "\n";
    color.resetColor();

    color.printLine('-', 40, GRAY);

    int rank = 1;
    for (const auto& [name, stat] : sortedPlayers) {
        if (rank == 1) color.setColor(YELLOW);
        else if (rank == 2) color.setColor(WHITE);
        else if (rank == 3) color.setColor(RED);
        else color.setColor(GRAY);

        std::cout << std::left << std::setw(5) << rank;

        color.setColor(GREEN);
        std::cout << std::setw(15) << name;

        color.setColor(CYAN);
        std::cout << std::setw(10) << stat.wins;

        color.setColor(RED);
        std::cout << std::setw(10) << stat.losses;

        color.resetColor();
        std::cout << "\n";
        rank++;
    }

    color.printLine('=', 40, CYAN);
    std::cout << "\n";
}

void viewHistory(const std::string& playerFilter = "") {
    std::cout << "\n";
    color.print("GAME HISTORY\n", CYAN);
    color.printLine('=', 30, GRAY);

    std::ifstream file("history.txt");
    if (!file.is_open()) {
        color.print("No history file found.\n", YELLOW);
        return;
    }

    std::string line;
    int lineNum = 1;
    bool hasContent = false;

    while (getline(file, line)) {
        if (playerFilter.empty() || line.find(playerFilter + " ") == 0) {
            hasContent = true;
            color.setColor(GRAY);
            std::cout << std::setw(3) << lineNum << ". ";
            color.resetColor();
            std::cout << line << "\n";
            lineNum++;
        }
    }

    if (!hasContent) {
        color.print("No history found", YELLOW);
        if (!playerFilter.empty())
            std::cout << " for player '" << playerFilter << "'";
        std::cout << ".\n";
    }

    color.printLine('=', 30, GRAY);
    std::cout << "\n";
}

void addWord(const std::string& word) {
    std::ofstream file("words.txt", std::ios::app);
    if (!file.is_open()) {
        color.print("Could not open words file!\n", RED);
        return;
    }

    file << word << "\n";
    color.print(" Word '" + word + "' added.\n", GREEN);
}

void resetLeaderboard() {
    std::ofstream("history.txt", std::ios::trunc);
    color.print("Leaderboard (history) has been reset.\n", YELLOW);
}

void addPlayer(const std::string& name) {
    std::ifstream check("history.txt");
    std::string line;
    while (getline(check, line)) {
        if (line.find(name + " ") == 0) {
            color.print("Player already exists in history.\n", YELLOW);
            return;
        }
    }

    std::ofstream file("history.txt", std::ios::app);
    color.print("Player '" + name + "' added to history.\n", GREEN);
}

void showUsage() {
    std::cout << "\n";
    color.print("HANGMAN MANAGER\n", MAGENTA);
    color.printLine('=', 35, CYAN);

    color.print("Commands:\n", WHITE);

    std::cout << "  ";
    color.print("view_leaderboard", CYAN);
    std::cout << "       - Show wins/losses from history\n";

    std::cout << "  ";
    color.print("view_history", CYAN);
    std::cout << " [name] - Show game history (optional player filter)\n";

    std::cout << "  ";
    color.print("add_word <word>", CYAN);
    std::cout << "         - Add word to dictionary\n";

    std::cout << "  ";
    color.print("add_player <name>", CYAN);
    std::cout << "      - Add a player to history\n";

    std::cout << "  ";
    color.print("reset_leaderboard", CYAN);
    std::cout << "     - Clear all history (reset leaderboard)\n";

    color.printLine('=', 35, CYAN);
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        color.print("✗ Error: Missing command\n", RED);
        showUsage();
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "view_leaderboard") {
        displayLeaderboardFromHistory();
    }
    else if (cmd == "view_history") {
        if (argc == 3)
            viewHistory(argv[2]);
        else
            viewHistory();
    }
    else if (cmd == "add_word" && argc == 3) {
        addWord(argv[2]);
    }
    else if (cmd == "add_player" && argc == 3) {
        addPlayer(argv[2]);
    }
    else if (cmd == "reset_leaderboard") {
        resetLeaderboard();
    }
    else {
        color.print("Unknown command.\n", RED);
        showUsage();
    }

    return 0;
}
