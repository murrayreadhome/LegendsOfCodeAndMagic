#include "GameController.h"
#include "CardInfo.h"


GameController::GameController(int seed)
    : draft(generate_draft(seed)), 
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
    Player* players[2] = { &player1, &player2 };

    for (int p = 0; p < 2; p++)
        player_states[p] = { 30, 0, 0, 5, 1 };

    // draw phase
    draw_phase = true;

    for (int i = 0; i < 30; i++)
    {
        for (int p = 0; p < 2; p++)
            player_states[p].deck = i;

        HandCards cards;
        for (Card c : draft[i])
        {
            c.location = 0;
            cards.push_back(c);
        }

        for (int p = 0; p < 2; p++)
        {
            hands[p] = cards;
            Action take = players[p]->draftAction(rules[p].visible_state());
            if (rules[p].can(take) && take.what == Action::PICK)
                decks[p].push_back(cards[take.a]);
            else
                cerr << "illegal take " << take << endl;
        }
    }

    // battle phase
    draw_phase = false;

    return 0;
}

