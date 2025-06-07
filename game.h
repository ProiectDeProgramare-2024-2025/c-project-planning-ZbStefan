//
// Created by Acer on 07.06.2025.
//

#ifndef GAME_H
#define GAME_H
class Player {
public:
    std::string name;
    int score;
    int games_played;

    Player(std::string n, int s, int g) : name(n), score(s), games_played(g) {}
};

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
#endif //GAME_H
