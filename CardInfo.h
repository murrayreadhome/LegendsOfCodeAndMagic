#pragma once
#include "LegendsOfCodeAndMagic.h"
#include <string>
#include <vector>


extern const CardInfo card_info[160];

std::vector<std::vector<Card>> generate_draft(int seed);
