#pragma once
#include "LegendsOfCodeAndMagic.h"
#include <string>
#include <vector>

struct CardInfo
{
    int id;
    std::string name;
    int type;
    int cost;
    int damage;
    int health;
    std::string abilities;
    int playerHP;
    int enemyHP;
    int cardDraw;
    std::string description;

    Card to_card() const;
};

extern const CardInfo card_info[160];

std::vector<std::vector<Card>> generate_draft(int seed);
