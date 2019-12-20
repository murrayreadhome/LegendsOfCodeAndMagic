#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <type_traits>

using namespace std;

#ifdef NDEBUG
#define check(b)
#else
void check(bool b) 
{
    if (!b)
        throw std::runtime_error("check failed");
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

    void push_back(const T& t) { check(!full()); (*this)[n++] = t; }
    void pop_back() { check(!empty()); n--; }

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

struct VisibleState
{
    bool draw_phase;
    PlayerState me;
    PlayerState op;
    HandCards myHand;
    BoardCards myBoard;
    BoardCards opBoard;

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

    void act(const Action & action)
    {
        switch (action.what)
        {
        case Action::SUMMON:
        {
            if (myBoard.full())
                return;
            Card* card = get_card(myHand, action.a);
            if (!card)
                return;
            card->used = !card->charge;
            myBoard.push_back(*card);
            me.mana -= card->cost;
            remove_card(*card, myHand);
        }
        break;
        case Action::ATTACK:
        {
            Card* card = get_card(myBoard, action.a);
            if (!card)
                return;

            card->used = true;
            if (action.b == -1)
            {
                op.health -= card->attack;
                return;
            }

            Card* opCard = get_card(opBoard, action.b);
            if (!opCard)
                return;

            bool blocked = opCard->ward;
            opCard->ward = false;

            if (blocked)
                return;

            card->defense -= opCard->attack;
            int damage = min(card->attack, opCard->defense);
            opCard->defense -= damage;

            if (card->lethal)
                opCard->defense = 0;
            if (card->breakthrough)
                op.health -= card->attack - damage;
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
            Card* card = get_card(myHand, action.a);
            if (!card)
                return;
            me.mana -= card->cost;
            
            if (action.b == -1)
            {
            }
            else if (card->cardType == Red || card->defense < 0)
            {
                Card* opCard = get_card(opBoard, action.b);
                if (!opCard)
                    return;
                red_effect(*card, *opCard);
                if (opCard->defense <= 0)
                    remove_card(*opCard, opBoard);
            }
            else
            {
                Card* myCard = get_card(myBoard, action.b);
                if (!myCard)
                    return;
                green_effect(*card, *myCard);
            }

            me.health += card->myHealthChange;
            op.health += card->opponentHealthChange;
            remove_card(*card, myHand);
        }
        break;
        case Action::PICK:
        case Action::PASS:
        default:
            break;
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

istream& operator>>(istream& in, PlayerState& player)
{
    in >> player.health >> player.mana >> player.deck >> player.rune >> player.draw;
    in.ignore();
    return in;
}

istream& operator>>(istream& in, Card& card)
{
    string abilities;
    in >> card.number >> card.instanceId >> card.location >> card.cardType >> card.cost >> card.attack >> card.defense >> abilities >> card.myHealthChange >> card.opponentHealthChange >> card.cardDraw;
    card.decode(abilities);
    card.used = false;
    in.ignore();
    return in;
}

istream& operator>>(istream& in, VisibleState& state)
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

ostream& operator<<(ostream& out, const Action& action)
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

class Player
{
public:
    Deck deck;
    vector<int> cost_counts;
    const vector<int> wanted = { 0, 6, 5, 4, 4, 4, 0, 0 };

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
        double d = c.defense;
        if (c.guard)
            d *= 2;
        double a = c.attack;
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
        double best_score = -1e9;
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

    vector<Action> battleAction(const VisibleState& in_state)
    {
        vector<Action> actions;
        VisibleState state = in_state;

        auto add_action = [&](const Action& action)
        {
            actions.push_back(action);
            state.act(action);
        };

        MaxVector<Card, 6> opGuard;
        MaxVector<Card, 6> opCreature;

        for (const Card& card : state.opBoard)
        {
            if (card.guard)
                opGuard.push_back(card);
            else
                opCreature.push_back(card);
        }

        // spend mana from most to least expensive card
        bool found = true;
        while (found)
        {
            sort(state.myHand.begin(), state.myHand.end(), [](const Card& a, const Card& b) { return a.cost > b.cost; });
            found = false;
            double current_score = score_state(state);

            for (Card& card : state.myHand)
            {
                switch (card.cardType)
                {
                case Creature:
                {
                    Action summon = { Action::SUMMON, card.instanceId };
                    if (state.can(summon))
                    {
                        add_action(summon);
                        found = true;
                    }
                    break;
                }
                case Green:
                {
                    auto best = find_best(state.myBoard.begin(), state.myBoard.end(), [&](const Card& myCard)
                    {
                        Action use = { Action::USE, card.instanceId, myCard.instanceId };
                        if (!state.can(use))
                            return -1.0;
                        VisibleState tmp_state = state;
                        tmp_state.act(use);
                        return score_state(tmp_state);
                    });
                    if (best.first != state.myBoard.end() && best.second > current_score)
                    {
                        Action use = { Action::USE, card.instanceId, best.first->instanceId };
                        add_action(use);
                        found = true;
                    }
                    break;
                }
                case Blue:
                    if (card.defense == 0)
                    {
                        Action use = { Action::USE, card.instanceId, -1 };
                        if (state.can(use))
                        {
                            add_action(use);
                            found = true;
                        }
                        break;
                    }
                    // else deliberate fall through
                case Red:
                {
                    auto best = find_best(state.opBoard.begin(), state.opBoard.end(), [&](const Card& opCard)
                    {
                        Action use = { Action::USE, card.instanceId, opCard.instanceId };
                        if (!state.can(use))
                            return -1.0;
                        VisibleState tmp_state = state;
                        tmp_state.act(use);
                        return score_state(tmp_state) - current_score;
                    });
                    if (best.first != state.opBoard.end() && best.second > 0)
                    {
                        Action use = { Action::USE, card.instanceId, best.first->instanceId };
                        add_action(use);
                        found = true;
                    }
                    break;
                }
                }
                if (found)
                    break;
            }
        }

        // attack guards
        while (!opGuard.empty())
        {
            pair<Action, double> attack = find_best_creature_attack(state, opGuard);
            if (attack.first.what != Action::ATTACK)
                break;
            add_action(attack.first);
        }

        // attack creatures
        for (;;)
        {
            pair<Action, double> attack = find_best_creature_attack(state, opCreature);
            if (attack.first.what != Action::ATTACK)
                break;
            if (attack.second < 0)
                break;
            add_action(attack.first);
        }

        // attack
        for (const Card& card : state.myBoard)
        {
            if (card.used)
                continue;
            Action face_hit = { Action::ATTACK, card.instanceId, -1 };
            add_action(face_hit);
        }

        if (actions.empty())
            actions.push_back({ Action::PASS });

        return actions;
    }
};

void codingame_loop()
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
