#pragma once
#include "LegendsOfCodeAndMagic.h"

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
    vector<vector<Card>> draft;
    int seed;

    bool draw_phase;
    PlayerState player_states[2];
    HandCards hands[2];
    BoardCards boards[2];
    Deck decks[2];
    MultiPlayerRules rules[2];
};

