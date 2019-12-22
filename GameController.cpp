#include "GameController.h"
#include "CardInfo.h"

namespace
{
    int other(int p)
    {
        return p == 0 ? 1 : 0;
    }
}


GameController::GameController(int seed)
    :
    seed(seed),
    rules
    {
        {draw_phase, player_states[0], player_states[1], hands[0], boards[0], boards[1]},
        {draw_phase, player_states[1], player_states[0], hands[1], boards[1], boards[0]}
    }
{
}

int GameController::play(Player& player1, Player& player2)
{
    players[0] = &player1;
    players[1] = &player2;

    draft();

    // battle phase
    int mana_bonus[2] = { 0, 1 };
    player_states[0].draw = 4;
    player_states[1].draw = 5;
    for (int p = 0; p < 2; p++)
    {
        draw(p);
        player_states[p].draw = 1;
    }


    for (int turn = 0; turn < 50; turn++)
    {
        for (int p = 0; p < 2; p++)
            player_states[p].mana = min(12, turn + 1) + mana_bonus[p];

        for (int p = 0; p < 2; p++)
        {
            draw(p);

            for (int p = 0; p < 2; p++)
            {
                for (Card& board_card : boards[p])
                    board_card.used = false;
            }

            player_states[p].deck = int(decks[p].size());

            int op = other(p);
//            cerr << rules[p].visible_state();
            vector<Action> actions = players[p]->battleAction(rules[p].visible_state());
            player_states[p].draw = 1;

            for (const Action& action : actions)
            {
//                cerr << action << endl;
                if (rules[p].can(action))
                {
                    char* error = rules[p].act(action);
                    if (error)
                        cerr << error << endl;
                    if (player_states[op].health <= 0)
                        return p;
                }
            }

            if (player_states[p].mana == 0)
                mana_bonus[p] = 0;
        }
    }

    if (player_states[1].health < player_states[0].health)
        return 0;
    else
        return 1;
}

void GameController::draw(int p)
{
    player_states[p].deck = int(decks[p].size());
    rules[p].draw_runes();

    for (int d = 0; d < player_states[p].draw; d++)
    {
        if (!hands[p].full() && !decks[p].empty())
        {
            hands[p].push_back(decks[p].back());
            decks[p].pop_back();
        }
    }
}

void GameController::draft()
{
    draw_phase = true;

    vector<vector<Card>> draft_choices = generate_draft(seed);

    for (int p = 0; p < 2; p++)
        player_states[p] = { 30, 0, 0, 25, 0 };

    for (int i = 0; i < 30; i++)
    {
        for (int p = 0; p < 2; p++)
            player_states[p].deck = i;

        HandCards cards;
        for (Card c : draft_choices[i])
        {
            c.location = 0;
            cards.push_back(c);
        }

        for (int p = 0; p < 2; p++)
        {
            hands[p] = cards;
//            cerr << rules[p].visible_state();
            Action take = players[p]->draftAction(rules[p].visible_state());
            if (rules[p].can(take) && take.what == Action::PICK)
                decks[p].push_back(cards[take.a]);
            else
                cerr << "illegal take " << take << endl;
        }
    }

    for (int p = 0; p < 2; p++)
    {
        hands[p].clear();
        shuffle(decks[p].begin(), decks[p].end(), re);
    }

    draw_phase = false;
}
