#pragma once
#include "LegendsOfCodeAndMagic.h"
#include <random>

struct MultiPlayerState
{
    bool& draw_phase;
    PlayerState& me;
    PlayerState& op;
    HandCards& myHand;
    BoardCards& myBoard;
    BoardCards& opBoard;

    VisibleState visible_state() const
    {
        return { draw_phase, me, op, myHand, myBoard, opBoard };
    }
};

typedef GameRules<MultiPlayerState> MultiPlayerRules;

class GameController
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

    bool draw_phase;
    PlayerState player_states[2];
    HandCards hands[2];
    BoardCards boards[2];
    Deck decks[2];
    MultiPlayerRules rules[2];
    Player* players[2];
};

