#pragma once
#include "LegendsOfCodeAndMagic.h"
#include <random>


class GameController : public FullState
{
public:
    GameController(int seed);

    int play(Player& player1, Player& player2);

private:
    void draft();
    void draw(int p);

private:
    int seed;
    mt19937 re;

    Player* players[2];
};

