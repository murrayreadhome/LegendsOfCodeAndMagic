#pragma once

#include "LegendsOfCodeAndMagic.h"

class OldPlayer1 : public Player
{
public:
    Deck deck;
    vector<int> cost_counts;
    const Params v;

    OldPlayer1(Params params = Params())
        : cost_counts(13, 0), v(params)
    {}

    double draft_card_value(const Card& card)
    {
        double value = v.d.draft_attack_m * abs(card.attack);
        value += v.d.draft_defense_m * abs(card.defense);
        value += v.d.draft_mult_m * (abs(card.attack) + v.d.card_m_a) * (abs(card.defense) + v.d.card_m_d);
        if (card.cardType == Creature)
        {
            value += v.d.creature;
            if (card.breakthrough) value += v.d.draft_attr_b;
            if (card.charge) value += v.d.draft_attr_c;
            if (card.drain) value += v.d.draft_attr_d;
            if (card.guard) value += v.d.draft_attr_g;
            if (card.lethal) value += v.d.draft_attr_l;
            if (card.ward) value += v.d.draft_attr_w;
        }
        else
        {
            value += v.d.draft_item_attack_m * abs(card.attack);
            value += v.d.draft_item_defense_m * min(double(abs(card.defense)), v.d.maxDamage);
            value += v.d.draft_item_mult_m * (abs(card.attack) + v.d.item_m_a) * (abs(card.defense) + v.d.item_m_d);
            if (card.breakthrough) value += v.d.draft_item_b;
            if (card.charge) value += v.d.draft_item_c;
            if (card.drain) value += v.d.draft_item_d;
            if (card.guard) value += v.d.draft_item_g;
            if (card.lethal) value += v.d.draft_item_l;
            if (card.ward) value += v.d.draft_item_w;
            if (card.cardType == Red) value += v.d.itemRed;
            if (card.cardType == Green) value += v.d.itemGreen;
            if (card.cardType == Blue) value += v.d.itemBlue;
        }
        value += v.d.draft_add_my_health * card.myHealthChange;
        value += v.d.draft_sub_op_health * card.opponentHealthChange;
        value += v.d.draft_draw * card.cardDraw;
        value *= pow(max(0.1, card.cost + v.d.cost_a), v.d.cost_pow);
        return value;
    }

    Action draftAction(const VisibleState& state)
    {
        int required = 0;
        for (int i = 0; i < 13; i++)
            required += max(0, v.i.curve[i] - cost_counts[i]);
        double flexibility = max(v.d.flex_min, 1.0 - v.d.flex_pen * required / (30 - deck.size()));

        auto best_card = find_best(state.myHand.begin(), state.myHand.end(), [&](const Card& card)
        {
            double value = draft_card_value(card);
            int wanted_value = v.i.curve[card.cost] - cost_counts[card.cost];
            if (wanted_value > 0)
                value += wanted_value;
            else
                value *= flexibility;
            return value;
        }).first;

        deck.push_back(*best_card);
        cost_counts[best_card->cost]++;
        return Action{ Action::PICK, int(best_card - state.myHand.begin()) };
    }

    double battle_card_value(const Card& card, bool opCard = false)
    {
        double value = v.d.attack_m * abs(card.attack);
        value += v.d.defense_m * abs(card.defense);
        value += v.d.mult_m * abs(card.attack) * abs(card.defense);
        if (card.breakthrough) value += v.d.attr_b;
        if (card.charge) value += v.d.attr_c;
        if (card.drain) value += v.d.attr_d;
        if (card.guard)
        {
            value += v.d.attr_g;
            if (card.breakthrough) value += v.d.attr_gb;
            if (card.drain) value += v.d.attr_gd;
            if (card.lethal) value += v.d.attr_gl;
            if (card.ward) value += v.d.attr_gw;
        }
        if (card.lethal) value += v.d.attr_l;
        if (card.ward) value += v.d.attr_w;
        if (card.used) value += v.d.attr_u;
        if (opCard)
        {
            if (card.breakthrough) value += v.d.attr_op_b;
            if (card.charge) value += v.d.attr_op_c;
            if (card.drain) value += v.d.attr_op_d;
            if (card.guard) value += v.d.attr_op_g;
            if (card.lethal) value += v.d.attr_op_l;
            if (card.ward) value += v.d.attr_op_w;
        }
        return value;
    }

    double score_state(const VisibleState& state)
    {
        double score = v.d.health * (state.me.health - state.op.health);
        if (state.op.health <= 0)
            score += 1e99;
        for (const Card& card : state.myHand)
            score += battle_card_value(card) * v.d.my_hand;
        for (const Card& card : state.myBoard)
            score += battle_card_value(card) * v.d.my_board;
        for (const Card& card : state.opBoard)
            score -= battle_card_value(card, true) * v.d.op_board;
        score += v.d.num_board * (state.myBoard.size() - state.opBoard.size());
        score += ((v.d.draw_30 * state.me.draw * state.me.deck) + (v.d.draw_0 * state.me.draw * (30 - state.me.deck))) / 30;
        return score;
    }

    pair<Action, double> find_best_creature_attack(const VisibleState& state, BoardCards& opBoard)
    {
        double best_score = bad_score;
        Action best_action = { Action::PASS };
        double init_score = score_state(state);
        for (const Card& my : state.myBoard)
        {
            if (my.used)
                continue;

            for (const Card& op : opBoard)
            {
                Action attack = { Action::ATTACK, my.instanceId, op.instanceId };
                if (!state.can(attack))
                    continue;
                VisibleState tmp = state;
                tmp.act(attack);
                double score = score_state(tmp) - init_score;
                if (score > best_score)
                {
                    best_score = score;
                    best_action = attack;
                }
            }
        }
        return { best_action, best_score };
    }

    pair<vector<Action>, double> find_best_creature_kill(const VisibleState& state, BoardCards& opBoard)
    {
        vector<Action> best_actions, actions;
        double best_score = bad_score;
        double init_score = score_state(state);

        MaxVector<int, 6> attackers;
        int max_attack = 0;
        for (const Card& card : state.myBoard)
        {
            if (card.used)
                continue;
            max_attack += card.attack;
            if (card.lethal)
                max_attack += 100;
            attackers.push_back(card.instanceId);
        }

        for (const Card& op : opBoard)
        {
            if (max_attack < op.defense)
                continue;
            int opId = op.instanceId;
            sort(attackers.begin(), attackers.end());
            do
            {
                VisibleState tmp = state;
                actions.clear();
                bool killed = false;
                for (int id : attackers)
                {
                    Action attack = { Action::ATTACK, id, opId };
                    if (!tmp.can(attack))
                        break;
                    actions.push_back(attack);
                    tmp.act(attack);
                    if (!get_card(tmp.opBoard, opId))
                    {
                        killed = true;
                        break;
                    }
                }
                double score = score_state(tmp) - init_score;
                if (score > best_score)
                {
                    best_score = score;
                    best_actions = actions;
                }
            } while (next_permutation(attackers.begin(), attackers.end()));
        }

        return { best_actions, best_score };
    }

    void play_greedy_card(StateChange& change, const Card& card)
    {
        switch (card.cardType)
        {
        case Creature:
        {
            Action summon = { Action::SUMMON, card.instanceId };
            if (change.state.can(summon))
            {
                change.add(summon);
            }
            break;
        }
        case Green:
        {
            auto best = find_best(change.state.myBoard.begin(), change.state.myBoard.end(), [&](const Card& myCard)
            {
                Action use = { Action::USE, card.instanceId, myCard.instanceId };
                if (!change.state.can(use))
                    return bad_score;
                VisibleState tmp_state = change.state;
                tmp_state.act(use);
                return score_state(tmp_state);
            });
            if (best.second > bad_score&& best.first != change.state.myBoard.end())
            {
                Action use = { Action::USE, card.instanceId, best.first->instanceId };
                change.add(use);
            }
            break;
        }
        case Blue:
            if (card.defense == 0)
            {
                Action use = { Action::USE, card.instanceId, -1 };
                if (change.state.can(use))
                {
                    change.add(use);
                }
                break;
            }
            // else deliberate fall through
        case Red:
        {
            auto best = find_best(change.state.opBoard.begin(), change.state.opBoard.end(), [&](const Card& opCard)
            {
                Action use = { Action::USE, card.instanceId, opCard.instanceId };
                if (!change.state.can(use))
                    return bad_score;
                VisibleState tmp_state = change.state;
                tmp_state.act(use);
                return score_state(tmp_state);
            });
            if (best.second > bad_score&& best.first != change.state.opBoard.end())
            {
                Action use = { Action::USE, card.instanceId, best.first->instanceId };
                change.add(use);
            }
            break;
        }
        }
    }

    pair<vector<Action>, double> greedy_spend(const VisibleState& in_state)
    {
        StateChange change{ in_state };

        bool found = true;
        while (found)
        {
            sort(change.state.myHand.begin(), change.state.myHand.end(), [](const Card& a, const Card& b) { return a.cost > b.cost; });
            found = false;
            double current_score = score_state(change.state);

            for (Card& card : change.state.myHand)
            {
                size_t before = change.actions.size();
                play_greedy_card(change, card);
                found = change.actions.size() > before;
                if (found)
                    break;
            }
        }

        return { change.actions, score_state(change.state) };
    }

    pair<vector<Action>, double> deploy_cards(const VisibleState& in_state, HandCards cards)
    {
        StateChange change{ in_state };

        // creatures first
        sort(cards.begin(), cards.end(), [](const Card& a, const Card& b) { return a.cardType < b.cardType; });
        for (const Card& card : cards)
            play_greedy_card(change, card);

        return { change.actions, score_state(change.state) };
    }

    vector<Action> find_best_spend(const VisibleState& in_state)
    {
        vector<Action> best_actions;
        double best_score;
        tie(best_actions, best_score) = greedy_spend(in_state);

        struct BestCards
        {
            double score;
            HandCards cards;
        };

        int m = in_state.me.mana;
        vector<BestCards> best_cards(m + 1, { bad_score });

        HandCards held = in_state.myHand;
        sort(held.begin(), held.end(), [](const Card& a, const Card& b)
        {
            if (a.cardType != b.cardType)
                return a.cardType < b.cardType;
            else
                return a.cost < b.cost;
        });
        for (const Card& card : held)
        {
            for (int i = m - card.cost; i >= 0; i--)
            {
                const BestCards& from = best_cards[i];
                if (i > 0 && from.cards.empty())
                    continue;

                HandCards new_cards = from.cards;
                if (new_cards.full())
                    continue;

                new_cards.push_back(card);
                pair<vector<Action>, double> deployment = deploy_cards(in_state, new_cards);

                BestCards& to = best_cards[i + card.cost];
                if (to.cards.empty() || to.score < deployment.second)
                {
                    to.cards = new_cards;
                    to.score = deployment.second;
                }

                if (deployment.second > best_score)
                {
                    best_score = deployment.second;
                    best_actions = deployment.first;
                }
            }
        }

        return best_actions;
    }

    vector<Action> battleAction(const VisibleState& in_state)
    {
        StateChange change{ in_state };

        // spend mana from most to least expensive card
        vector<Action> spend = find_best_spend(change.state);
        change.add(spend);

        // attack guards
        MaxVector<Card, 6> opGuard;
        MaxVector<Card, 6> opCreature;

        for (const Card& card : change.state.opBoard)
        {
            if (card.guard)
                opGuard.push_back(card);
            else
                opCreature.push_back(card);
        }

        while (!opGuard.empty())
        {
            pair<vector<Action>, double> attack = find_best_creature_kill(change.state, opGuard);
            if (attack.first.empty())
            {
                pair<Action, double> attack = find_best_creature_attack(change.state, opGuard);
                if (attack.first.what != Action::ATTACK)
                    break;
                change.add(attack.first);
            }
            else
            {
                change.add(attack.first);
            }
        }

        // split the remaining attacks
        double best_score = bad_score;
        vector<Action> best_attacks;
        vector<Action> attacks;
        for (;;)
        {
            // try the current attack actions, then face hit
            StateChange attack_change{ change.state };
            attack_change.add(attacks);
            StateChange post_attack{ attack_change.state };
            for (const Card& card : attack_change.state.myBoard)
            {
                if (card.used)
                    continue;
                Action face_hit = { Action::ATTACK, card.instanceId, -1 };
                attack_change.add(face_hit);
            }

            // score that
            double score = score_state(attack_change.state);
            if (score > best_score)
            {
                best_score = score;
                best_attacks = attack_change.actions;
            }

            // try to add another creature attack
            pair<vector<Action>, double> attack = find_best_creature_kill(post_attack.state, opCreature);
            if (attack.first.empty())
                break;
            if (attack.second < 0)
                break;
            for (auto action : attack.first)
                attacks.push_back(action);
        }
        change.add(best_attacks);

        if (change.actions.empty())
            change.actions.push_back({ Action::PASS });

        return change.actions;
    }
};
