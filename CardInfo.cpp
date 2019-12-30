#include "CardInfo.h"
#include <random>


std::vector<std::vector<Card>> generate_draft(int seed)
{
    mt19937 re(seed);

    vector<Card> pool;
    for (int i = 0; i < 60; i++)
    {
        int n = re() % 160;
        pool.push_back(card_info[n].to_card());
    }

    int instanceId = 0;
    vector<vector<Card>> draft;
    for (int i = 0; i < 30; i++)
    {
        vector<Card> round;
        for (int j = 0; j < 3; j++)
        {
            bool ok = false;
            Card c;
            while (!ok)
            {
                c = pool[re() % 60];
                ok = true;
                for (int k = 0; k < j; k++)
                {
                    if (round[k].number == c.number)
                        ok = false;
                }
            }
            c.instanceId = instanceId++;
            round.push_back(c);
        }
        draft.push_back(round);
    }

    return draft;
}

