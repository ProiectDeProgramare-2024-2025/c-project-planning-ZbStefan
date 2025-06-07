#include <string>
#include <vector>

#ifndef INFO_H
#define INFO_H

class Player {
public:
    std::string name;
    int score;
    int games_played;

    Player(std::string n, int s, int g) : name(n), score(s), games_played(g) {}
};

#endif //INFO_H
