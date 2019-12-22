#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <type_traits>

using namespace std;

#ifdef NDEBUG
#define check(b, msg)
#else
inline void check(bool b, char* msg)
{
    if (!b)
    {
        cerr << msg << endl;
        throw std::runtime_error("check failed");
    }
};
#endif


template<typename T, size_t N>
class MaxVector : public array<T, N>
{
public:
    MaxVector() = default;
    ~MaxVector() = default;

    size_t size() const { return n; }
    bool empty() const { return n == 0; }
    bool full() const { return n == N; }

    void clear() { n = 0; }

    void push_back(const T& t) { check(!full(), "full"); (*this)[n++] = t; }
    void pop_back() { check(!empty(), "empty"); n--; }

    T& back() { return (*this)[n - 1]; }
    const T& back() const { return (*this)[n - 1]; }

    typedef typename array<T, N>::iterator iterator;
    typedef typename array<T, N>::const_iterator const_iterator;
    iterator end() { return this->begin() + n; }
    const_iterator end() const { return this->begin() + n; }
    const_iterator cend() const { return this->begin() + n; }

private:
    size_t n = 0;
};

struct PlayerState
{
    int health;
    int mana;
    int deck;
    int rune;
    int draw;
};

enum Location { OpBoard = -1, MyHand = 0, MyBoard = 1 };
enum CardType { Creature, Green, Red, Blue };

struct Card
{
    int number;
    int instanceId;
    int location;
    int cardType;
    int cost;
    int attack;
    int defense;
    int myHealthChange;
    int opponentHealthChange;
    int cardDraw;

    int cost_class() const
    {
        return min(7, cost);
    }

    bool breakthrough = false;
    bool charge = false;
    bool guard = false;
    bool drain = false;
    bool lethal = false;
    bool ward = false;
    bool used = false;

    void decode(const string& abilities)
    {
        for (char c : abilities)
        {
            switch (c)
            {
            case 'B':
                breakthrough = true;
                break;
            case 'C':
                charge = true;
                break;
            case 'D':
                drain = true;
                break;
            case 'G':
                guard = true;
                break;
            case 'L':
                lethal = true;
                break;
            case 'W':
                ward = true;
                break;
            default:
                break;
            }
        }
    }

    void encode(string& abilities) const
    {
        abilities.push_back(breakthrough ? 'B' : '-');
        abilities.push_back(charge ? 'C' : '-');
        abilities.push_back(drain ? 'D' : '-');
        abilities.push_back(guard ? 'G' : '-');
        abilities.push_back(lethal ? 'L' : '-');
        abilities.push_back(ward ? 'W' : '-');
    }
};

struct Action
{
    enum What { PICK, SUMMON, ATTACK, USE, PASS } what;
    int a;
    int b = 0;
};

template<typename T>
void remove_card(Card& card, T& cards)
{
    swap(card, cards.back());
    cards.pop_back();
}

typedef MaxVector<Card, 6> BoardCards;
typedef MaxVector<Card, 8> HandCards;

struct SinglePlayerState
{
    bool draw_phase;
    PlayerState me;
    PlayerState op;
    HandCards myHand;
    BoardCards myBoard;
    BoardCards opBoard;
};

template <typename T>
struct GameRules : public T
{
    using T::draw_phase;
    using T::me;
    using T::op;
    using T::myHand;
    using T::myBoard;
    using T::opBoard;

    GameRules()
    {}

    GameRules(bool& draw_phase, PlayerState& me, PlayerState& op, HandCards& myHand, BoardCards& myBoard, BoardCards& opBoard)
        : T{ draw_phase, me, op, myHand, myBoard, opBoard }
    {}

    bool can(const Action& action) const
    {
        if ((action.what == Action::PICK) != draw_phase)
            return false;
        
        switch (action.what)
        {
        case Action::SUMMON:
        {
            if (myBoard.full())
                return false;
            const Card* card = get_card(myHand, action.a);
            if (!card)
                return false;
            if (card->cardType != Creature)
                return false;
            return card->cost <= me.mana;
        }
        break;
        case Action::ATTACK:
        {
            const Card* card = get_card(myBoard, action.a);
            if (!card)
                return false;
            if (card->used)
                return false;
            if (card->cardType != Creature)
                return false;
            if (blocked_by_guard(action.b))
                return false;
            return true;
        }
        break;
        case Action::USE:
        {
            const Card* card = get_card(myHand, action.a);
            if (!card)
                return false;
            return (card->cardType != Creature && card->cost <= me.mana);
        }
        break;
        case Action::PICK:
        case Action::PASS:
        default:
            return true;
        }
    }

    bool blocked_by_guard(int target) const
    {
        if (target != -1)
        {
            const Card* card = get_card(opBoard, target);
            if (!card)
                return true;
            if (card->guard)
                return false;
        }
        size_t guards = count_if(opBoard.begin(), opBoard.end(), [](const Card& card)
        {
            return card.guard;
        });
        return guards > 0;
    }

    template <typename C>
    const Card* get_card(const C& cards, int id) const
    {
        auto pcard = find_if(cards.begin(), cards.end(), [id](const Card& card)
        {
            return card.instanceId == id;
        });
        if (pcard == cards.end())
            return nullptr;
        else
            return &*pcard;
    }

    template <typename C>
    Card* get_card(C& cards, int id)
    {
        auto pcard = find_if(cards.begin(), cards.end(), [id](Card& card)
        {
            return card.instanceId == id;
        });
        if (pcard == cards.end())
            return nullptr;
        else
            return &*pcard;
    }

    char* act(const Action & action)
    {
        if (action.what == Action::PASS || action.what == Action::PICK)
            return nullptr;

        Card* card;
        if (action.what == Action::ATTACK)
            card = get_card(myBoard, action.a);
        else
            card = get_card(myHand, action.a);
        if (!card)
            return "action source card not found";

        switch (action.what)
        {
        case Action::SUMMON:
        {
            if (myBoard.full())
                return "board full";
            card->used = !card->charge;
            myBoard.push_back(*card);
        }
        break;

        case Action::ATTACK:
        {
            card->used = true;
            if (action.b == -1)
            {
                op.health -= card->attack;
                op_runes();
                return nullptr;
            }

            Card* opCard = get_card(opBoard, action.b);
            if (!opCard)
                return "opponent attack target not found";

            bool blocked = opCard->ward;
            if (opCard->attack > 0)
                opCard->ward = false;

            if (blocked)
                return nullptr;

            card->defense -= opCard->attack;
            int damage = min(card->attack, opCard->defense);
            opCard->defense -= damage;

            if (card->lethal)
                opCard->defense = 0;
            if (card->breakthrough)
            {
                op.health -= card->attack - damage;
                op_runes();
            }
            if (card->drain)
                me.health += damage;

            if (card->defense <= 0)
                remove_card(*card, myBoard);

            if (opCard->defense <= 0)
                remove_card(*opCard, opBoard);
        }
        break;

        case Action::USE:
        {
            if (action.b == -1)
            {
            }
            else if (card->cardType == Red || card->defense < 0)
            {
                Card* opCard = get_card(opBoard, action.b);
                if (!opCard)
                    return "opponent use target not found";
                red_effect(*card, *opCard);
                if (opCard->defense <= 0)
                    remove_card(*opCard, opBoard);
            }
            else
            {
                Card* myCard = get_card(myBoard, action.b);
                if (!myCard)
                    return "my use target not found";
                green_effect(*card, *myCard);
            }
        }
        break;

        default:
            break;
        }

        if (action.what == Action::SUMMON || action.what == Action::USE)
        {
            me.health += card->myHealthChange;
            op.health += card->opponentHealthChange;
            op_runes();
            me.mana -= card->cost;
            me.draw += card->cardDraw;
            remove_card(*card, myHand);
        }

        return nullptr;
    }

    void op_runes()
    {
        while (op.health < op.rune)
        {
            op.rune -= 5;
            op.draw += 1;
        }
    }

    void draw_runes()
    {
        int deck = me.deck;
        for (int d = 0; d < me.draw; d++)
        {
            if (deck <= 0)
            {
                me.health = min(me.health, me.rune);
                me.rune -= 5;
                return;
            }
            deck--;
        }
    }

    void green_effect(const Card& effect, Card& card)
    {
        card.attack += effect.attack;
        card.defense += effect.defense;
        card.breakthrough = card.breakthrough || effect.breakthrough;
        card.charge = card.charge || effect.charge;
        card.drain = card.drain || effect.drain;
        card.guard = card.guard || effect.guard;
        card.lethal = card.lethal || effect.lethal;
        card.ward = card.ward || effect.ward;
    }

    void red_effect(const Card& effect, Card& card)
    {
        card.attack += effect.attack;
        card.defense += effect.defense;
        card.breakthrough = card.breakthrough && !effect.breakthrough;
        card.charge = card.charge && !effect.charge;
        card.drain = card.drain && !effect.drain;
        card.guard = card.guard && !effect.guard;
        card.lethal = card.lethal && !effect.lethal;
        card.ward = card.ward && !effect.ward;
    }
};

typedef GameRules<SinglePlayerState> VisibleState;

inline istream& operator>>(istream& in, PlayerState& player)
{
    in >> player.health >> player.mana >> player.deck >> player.rune >> player.draw;
    in.ignore();
    return in;
}

inline ostream& operator<<(ostream& out, const PlayerState& player)
{
    out << player.health << " " << player.mana << " " << player.deck << " " << player.rune << " " << player.draw << endl;
    return out;
}

inline istream& operator>>(istream& in, Card& card)
{
    string abilities;
    in >> card.number >> card.instanceId >> card.location >> card.cardType >> card.cost >> card.attack >> card.defense >> abilities >> card.myHealthChange >> card.opponentHealthChange >> card.cardDraw;
    card.decode(abilities);
    card.used = false;
    in.ignore();
    return in;
}

inline ostream& operator<<(ostream& out, const Card& card)
{
    string abilities;
    card.encode(abilities);
    out << card.number << " " << card.instanceId << " " << card.location << " " << card.cardType << " " << card.cost << " " << card.attack << " " << card.defense << " " << abilities << " " << card.myHealthChange << " " << card.opponentHealthChange << " " << card.cardDraw;
    return out;
}

inline istream& operator>>(istream& in, VisibleState& state)
{
    in >> state.me;
    in >> state.op;

    int opponentHand;
    int opponentActions;
    cin >> opponentHand >> opponentActions; cin.ignore();
    for (int i = 0; i < opponentActions; i++) {
        string cardNumberAndAction;
        getline(cin, cardNumberAndAction);
    }
    int cardCount;
    cin >> cardCount; cin.ignore();

    for (int i = 0; i < cardCount; i++) {
        Card card;
        cin >> card;
        switch (card.location)
        {
        case OpBoard:
            state.opBoard.push_back(card);
            break;
        case MyHand:
            state.myHand.push_back(card);
            break;
        case MyBoard:
            state.myBoard.push_back(card);
            break;
        default:
            break;
        }
    }

    state.draw_phase = state.me.mana == 0;

    return in;
}

inline ostream& operator<<(ostream& out, const VisibleState& state)
{
    out << state.me;
    out << state.op;

    vector<Card> cards;
    cards.insert(cards.end(), state.opBoard.begin(), state.opBoard.end());
    cards.insert(cards.end(), state.myHand.begin(), state.myHand.end());
    cards.insert(cards.end(), state.myBoard.begin(), state.myBoard.end());

    out << cards.size() << endl;
    for (Card card : cards)
        out << card << endl;

    return out;
}

inline ostream& operator<<(ostream& out, const Action& action)
{
    const char* names[] = { "PICK", "SUMMON", "ATTACK", "USE", "PASS" };
    out << names[action.what];
    if (action.what != Action::PASS)
        out << " " << action.a;
    if (action.what == Action::ATTACK || action.what == Action::USE)
        out << " " << action.b;
    return out;
}

typedef MaxVector<Card, 30> Deck;

template<typename It, typename Eval>
auto find_best(It begin, It end, Eval eval) -> pair<It, decltype(eval(*begin))>
{
    if (begin == end)
        return { end, {} };
    auto bestSc = eval(*begin);
    It best = begin;
    ++begin;
    for (; begin != end; ++begin)
    {
        auto sc = eval(*begin);
        if (sc > bestSc)
        {
            bestSc = sc;
            best = begin;
        }
    }
    return { best, bestSc };
}

struct StateChange
{
    VisibleState state;
    vector<Action> actions;

    void add(const Action& action)
    {
        actions.push_back(action);
        state.act(action);
    }

    template<typename T>
    void add(const T& actions)
    {
        for (const Action& action : actions)
            add(action);
    }
};

const double bad_score = -1e9;

class Player
{
public:
    Deck deck;
    vector<int> cost_counts;
    const vector<int> wanted = { 0, 5, 4, 3, 3, 3, 0, 0 };

    Player()
        : cost_counts(8, 0)
    {}

    Action draftAction(const VisibleState& state)
    {
        int required = 0;
        for (int i = 0; i < 8; i++)
            required += max(0, wanted[i] - cost_counts[i]);
        double flexibility = max(0.0001, 1.0 - 1.0 * required / (30 - deck.size()));

        auto best_card = find_best(state.myHand.begin(), state.myHand.end(), [&](const Card& card) 
        {
            double value = 1.0 * card.attack * card.defense / (card.cost + 1);
            int wanted_value = wanted[card.cost_class()] - cost_counts[card.cost_class()];
            if (wanted_value > 0)
                value += wanted_value;
            else
                value *= flexibility;
            return value;
        }).first;

        deck.push_back(*best_card);
        cost_counts[best_card->cost_class()]++;
        return Action{ Action::PICK, int(best_card - state.myHand.begin()) };
    }

    double card_value(const Card& c)
    {
        double d = max(1, c.defense);
        if (c.guard)
            d *= 2;
        double a = max(1, c.attack);
        if (c.lethal)
            a = max(a, 5.0);
        if (c.drain)
            a *= 2;
        if (c.breakthrough)
            a *= 2;
        return d * a;
    }

    double score_state(const VisibleState& state)
    {
        double score = (state.me.health - state.op.health);
        if (state.op.health <= 0)
            score += 1000;
        for (const Card& card : state.myHand)
            score += card_value(card) * 0.5;
        for (const Card& card : state.myBoard)
            score += card_value(card);
        for (const Card& card : state.opBoard)
            score -= card_value(card);
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

        MaxVector<int,6> attackers;
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
                    if (!tmp.get_card(tmp.opBoard, opId))
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
            if (best.second > bad_score && best.first != change.state.myBoard.end())
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
            if (best.second > bad_score && best.first != change.state.opBoard.end())
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
            for (int i = m - card.cost; i>=0; i--)
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

inline void codingame_loop()
{
    Player player;

    // game loop
    while (1) {
        VisibleState vs;
        cin >> vs;

        vector<Action> actions;
        if (vs.draw_phase)
            actions.push_back(player.draftAction(vs));
        else
            actions = player.battleAction(vs);

        bool first = true;
        for (const Action& action : actions)
        {
            if (!first)
                cout << ";";
            first = false;
            cout << action;
        }
        cout << endl;
    }
}

#ifndef DEVELOPMENT
int main()
{
    codingame_loop();
    return 0;
}
#endif
