#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <type_traits>
#include <cmath>
#include <chrono>
#include <sstream>

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

#define DIFF(n) if (n != c.n) { stringstream strm; strm << #n << n << "!=" << c.n << " "; return strm.str(); }

struct PlayerState
{
    int health;
    int mana;
    int deck;
    int rune;
    int draw;
    string diff(const PlayerState& c) const
    {
        DIFF(health);
        DIFF(rune);
        return "";
    }
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

    bool breakthrough = false;
    bool charge = false;
    bool guard = false;
    bool drain = false;
    bool lethal = false;
    bool ward = false;
    bool used = false;
    bool new_this_turn = false;

    string diff(const Card& c) const
    {
        DIFF(number)
            DIFF(instanceId)
            DIFF(cardType)
            DIFF(cost)
            DIFF(attack)
            DIFF(defense)
            DIFF(breakthrough)
            DIFF(charge)
            DIFF(guard)
            DIFF(drain)
            DIFF(lethal)
            DIFF(ward)
            return "";
    }

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

    Card to_card() const
    {
        Card c;
        c.number = id;
        c.cardType = type;
        c.cost = cost;
        c.attack = damage;
        c.defense = health;
        c.decode(abilities);
        c.myHealthChange = playerHP;
        c.opponentHealthChange = enemyHP;
        c.cardDraw = cardDraw;
        return c;
    }
};

static const CardInfo card_info[160] =
{
{ 1, "Slimer", Creature, 1, 2, 1, "------", 1, 0, 0, "Summon: You gain 1 health. " },
{ 2, "Scuttler", Creature, 1, 1, 2, "------", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 3, "Beavrat", Creature, 1, 2, 2, "------", 0, 0, 0, "" },
{ 4, "Plated Toad", Creature, 2, 1, 5, "------", 0, 0, 0, "" },
{ 5, "Grime Gnasher", Creature, 2, 4, 1, "------", 0, 0, 0, "" },
{ 6, "Murgling", Creature, 2, 3, 2, "------", 0, 0, 0, "" },
{ 7, "Rootkin Sapling", Creature, 2, 2, 2, "-----W", 0, 0, 0, "" },
{ 8, "Psyshroom", Creature, 2, 2, 3, "------", 0, 0, 0, "" },
{ 9, "Corrupted Beavrat", Creature, 3, 3, 4, "------", 0, 0, 0, "" },
{ 10, "Carnivorous Bush", Creature, 3, 3, 1, "--D---", 0, 0, 0, "" },
{ 11, "Snowsaur", Creature, 3, 5, 2, "------", 0, 0, 0, "" },
{ 12, "Woodshroom", Creature, 3, 2, 5, "------", 0, 0, 0, "" },
{ 13, "Swamp Terror", Creature, 4, 5, 3, "------", 1, -1, 0, "Summon: You gain 1 health and deal\n1 damage to your opponent. " },
{ 14, "Fanged Lunger", Creature, 4, 9, 1, "------", 0, 0, 0, "" },
{ 15, "Pouncing Flailmouth", Creature, 4, 4, 5, "------", 0, 0, 0, "" },
{ 16, "Wrangler Fish", Creature, 4, 6, 2, "------", 0, 0, 0, "" },
{ 17, "Ash Walker", Creature, 4, 4, 5, "------", 0, 0, 0, "" },
{ 18, "Acid Golem", Creature, 4, 7, 4, "------", 0, 0, 0, "" },
{ 19, "Foulbeast", Creature, 5, 5, 6, "------", 0, 0, 0, "" },
{ 20, "Hedge Demon", Creature, 5, 8, 2, "------", 0, 0, 0, "" },
{ 21, "Crested Scuttler", Creature, 5, 6, 5, "------", 0, 0, 0, "" },
{ 22, "Sigbovak", Creature, 6, 7, 5, "------", 0, 0, 0, "" },
{ 23, "Titan Cave Hog", Creature, 7, 8, 8, "------", 0, 0, 0, "" },
{ 24, "Exploding Skitterbug", Creature, 1, 1, 1, "------", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 25, "Spiney Chompleaf", Creature, 2, 3, 1, "------", -2, -2, 0, "Summon: Deal 2 damage to each player. " },
{ 26, "Razor Crab", Creature, 2, 3, 2, "------", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 27, "Nut Gatherer", Creature, 2, 2, 2, "------", 2, 0, 0, "Summon: You gain 2 health. " },
{ 28, "Infested Toad", Creature, 2, 1, 2, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 29, "Steelplume Nestling", Creature, 2, 2, 1, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 30, "Venomous Bog Hopper", Creature, 3, 4, 2, "------", 0, -2, 0, "Summon: Deal 2 damage to your opponent. " },
{ 31, "Woodland Hunter", Creature, 3, 3, 1, "------", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 32, "Sandsplat", Creature, 3, 3, 2, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 33, "Chameleskulk", Creature, 4, 4, 3, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 34, "Eldritch Cyclops", Creature, 5, 3, 5, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 35, "Snail-eyed Hulker", Creature, 6, 5, 2, "B-----", 0, 0, 1, "Summon: Draw a card. " },
{ 36, "Possessed Skull", Creature, 6, 4, 4, "------", 0, 0, 2, "Summon: Draw two cards. " },
{ 37, "Eldritch Multiclops", Creature, 6, 5, 7, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 38, "Imp", Creature, 1, 1, 3, "--D---", 0, 0, 0, "" },
{ 39, "Voracious Imp", Creature, 1, 2, 1, "--D---", 0, 0, 0, "" },
{ 40, "Rock Gobbler", Creature, 3, 2, 3, "--DG--", 0, 0, 0, "" },
{ 41, "Blizzard Demon", Creature, 3, 2, 2, "-CD---", 0, 0, 0, "" },
{ 42, "Flying Leech", Creature, 4, 4, 2, "--D---", 0, 0, 0, "" },
{ 43, "Screeching Nightmare", Creature, 6, 5, 5, "--D---", 0, 0, 0, "" },
{ 44, "Deathstalker", Creature, 6, 3, 7, "--D-L-", 0, 0, 0, "" },
{ 45, "Night Howler", Creature, 6, 6, 5, "B-D---", -3, 0, 0, "Summon: You lose 3 health." },
{ 46, "Soul Devourer", Creature, 9, 7, 7, "--D---", 0, 0, 0, "" },
{ 47, "Gnipper", Creature, 2, 1, 5, "--D---", 0, 0, 0, "" },
{ 48, "Venom Hedgehog", Creature, 1, 1, 1, "----L-", 0, 0, 0, "" },
{ 49, "Shiny Prowler", Creature, 2, 1, 2, "---GL-", 0, 0, 0, "" },
{ 50, "Puff Biter", Creature, 3, 3, 2, "----L-", 0, 0, 0, "" },
{ 51, "Elite Bilespitter", Creature, 4, 3, 5, "----L-", 0, 0, 0, "" },
{ 52, "Bilespitter", Creature, 4, 2, 4, "----L-", 0, 0, 0, "" },
{ 53, "Possessed Abomination", Creature, 4, 1, 1, "-C--L-", 0, 0, 0, "" },
{ 54, "Shadow Biter", Creature, 3, 2, 2, "----L-", 0, 0, 0, "" },
{ 55, "Hermit Slime", Creature, 2, 0, 5, "---G--", 0, 0, 0, "" },
{ 56, "Giant Louse", Creature, 4, 2, 7, "------", 0, 0, 0, "" },
{ 57, "Dream-Eater", Creature, 4, 1, 8, "------", 0, 0, 0, "" },
{ 58, "Darkscale Predator", Creature, 6, 5, 6, "B-----", 0, 0, 0, "" },
{ 59, "Sea Ghost", Creature, 7, 7, 7, "------", 1, -1, 0, "Summon: You gain 1 health and deal\n1 damage to your opponent. " },
{ 60, "Gritsuck Troll", Creature, 7, 4, 8, "------", 0, 0, 0, "" },
{ 61, "Alpha Troll", Creature, 9, 10, 10, "------", 0, 0, 0, "" },
{ 62, "Mutant Troll", Creature, 12, 12, 12, "B--G--", 0, 0, 0, "" },
{ 63, "Rootkin Drone", Creature, 2, 0, 4, "---G-W", 0, 0, 0, "" },
{ 64, "Coppershell Tortoise", Creature, 2, 1, 1, "---G-W", 0, 0, 0, "" },
{ 65, "Steelplume Defender", Creature, 2, 2, 2, "-----W", 0, 0, 0, "" },
{ 66, "Staring Wickerbeast", Creature, 5, 5, 1, "-----W", 0, 0, 0, "" },
{ 67, "Flailing Hammerhead", Creature, 6, 5, 5, "-----W", 0, -2, 0, "Summon: Deal 2 damage to your opponent. " },
{ 68, "Giant Squid", Creature, 6, 7, 5, "-----W", 0, 0, 0, "" },
{ 69, "Charging Boarhound", Creature, 3, 4, 4, "B-----", 0, 0, 0, "" },
{ 70, "Murglord", Creature, 4, 6, 3, "B-----", 0, 0, 0, "" },
{ 71, "Flying Murgling", Creature, 4, 3, 2, "BC----", 0, 0, 0, "" },
{ 72, "Shuffling Nightmare", Creature, 4, 5, 3, "B-----", 0, 0, 0, "" },
{ 73, "Bog Bounder", Creature, 4, 4, 4, "B-----", 4, 0, 0, "Summon: You gain 4 health. " },
{ 74, "Crusher", Creature, 5, 5, 4, "B--G--", 0, 0, 0, "" },
{ 75, "Titan Prowler", Creature, 5, 6, 5, "B-----", 0, 0, 0, "" },
{ 76, "Crested Chomper", Creature, 6, 5, 5, "B-D---", 0, 0, 0, "" },
{ 77, "Lumbering Giant", Creature, 7, 7, 7, "B-----", 0, 0, 0, "" },
{ 78, "Shambler", Creature, 8, 5, 5, "B-----", 0, -5, 0, "Summon: Deal 5 damage to your opponent. " },
{ 79, "Scarlet Colossus", Creature, 8, 8, 8, "B-----", 0, 0, 0, "" },
{ 80, "Corpse Guzzler", Creature, 8, 8, 8, "B--G--", 0, 0, 1, "Summon: Draw a card. " },
{ 81, "Flying Corpse Guzzler", Creature, 9, 6, 6, "BC----", 0, 0, 0, "" },
{ 82, "Slithering Nightmare", Creature, 7, 5, 5, "B-D--W", 0, 0, 0, "" },
{ 83, "Restless Owl", Creature, 0, 1, 1, "-C----", 0, 0, 0, "" },
{ 84, "Fighter Tick", Creature, 2, 1, 1, "-CD--W", 0, 0, 0, "" },
{ 85, "Heartless Crow", Creature, 3, 2, 3, "-C----", 0, 0, 0, "" },
{ 86, "Crazed Nose-pincher", Creature, 3, 1, 5, "-C----", 0, 0, 0, "" },
{ 87, "Bloat Demon", Creature, 4, 2, 5, "-C-G--", 0, 0, 0, "" },
{ 88, "Abyss Nightmare", Creature, 5, 4, 4, "-C----", 0, 0, 0, "" },
{ 89, "Boombeak", Creature, 5, 4, 1, "-C----", 2, 0, 0, "Summon: You gain 2 health." },
{ 90, "Eldritch Swooper", Creature, 8, 5, 5, "-C----", 0, 0, 0, "" },
{ 91, "Flumpy", Creature, 0, 1, 2, "---G--", 0, 1, 0, "Summon: Your opponent gains 1 health." },
{ 92, "Wurm", Creature, 1, 0, 1, "---G--", 2, 0, 0, "Summon: You gain 2 health. " },
{ 93, "Spinekid", Creature, 1, 2, 1, "---G--", 0, 0, 0, "" },
{ 94, "Rootkin Defender", Creature, 2, 1, 4, "---G--", 0, 0, 0, "" },
{ 95, "Wildum", Creature, 2, 2, 3, "---G--", 0, 0, 0, "" },
{ 96, "Prairie Protector", Creature, 2, 3, 2, "---G--", 0, 0, 0, "" },
{ 97, "Turta", Creature, 3, 3, 3, "---G--", 0, 0, 0, "" },
{ 98, "Lilly Hopper", Creature, 3, 2, 4, "---G--", 0, 0, 0, "" },
{ 99, "Cave Crab", Creature, 3, 2, 5, "---G--", 0, 0, 0, "" },
{ 100, "Stalagopod", Creature, 3, 1, 6, "---G--", 0, 0, 0, "" },
{ 101, "Engulfer", Creature, 4, 3, 4, "---G--", 0, 0, 0, "" },
{ 102, "Mole Demon", Creature, 4, 3, 3, "---G--", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 103, "Mutating Rootkin", Creature, 4, 3, 6, "---G--", 0, 0, 0, "" },
{ 104, "Deepwater Shellcrab", Creature, 4, 4, 4, "---G--", 0, 0, 0, "" },
{ 105, "King Shellcrab", Creature, 5, 4, 6, "---G--", 0, 0, 0, "" },
{ 106, "Far-reaching Nightmare", Creature, 5, 5, 5, "---G--", 0, 0, 0, "" },
{ 107, "Worker Shellcrab", Creature, 5, 3, 3, "---G--", 3, 0, 0, "Summon: You gain 3 health. " },
{ 108, "Rootkin Elder", Creature, 5, 2, 6, "---G--", 0, 0, 0, "" },
{ 109, "Elder Engulfer", Creature, 5, 5, 6, "------", 0, 0, 0, "" },
{ 110, "Gargoyle", Creature, 5, 0, 9, "---G--", 0, 0, 0, "" },
{ 111, "Turta Knight", Creature, 6, 6, 6, "---G--", 0, 0, 0, "" },
{ 112, "Rootkin Leader", Creature, 6, 4, 7, "---G--", 0, 0, 0, "" },
{ 113, "Tamed Bilespitter", Creature, 6, 2, 4, "---G--", 4, 0, 0, "Summon: You gain 4 health. " },
{ 114, "Gargantua", Creature, 7, 7, 7, "---G--", 0, 0, 0, "" },
{ 115, "Rootkin Warchief", Creature, 8, 5, 5, "---G-W", 0, 0, 0, "" },
{ 116, "Emperor Nightmare", Creature, 12, 8, 8, "BCDGLW", 0, 0, 0, "" },
{ 117, "Protein", Green, 1, 1, 1, "B-----", 0, 0, 0, "Give a friendly creature +1/+1 and Breakthrough. " },
{ 118, "Royal Helm", Green, 0, 0, 3, "------", 0, 0, 0, "Give a friendly creature +0/+3. " },
{ 119, "Serrated Shield", Green, 1, 1, 2, "------", 0, 0, 0, "Give a friendly creature +1/+2. " },
{ 120, "Venomfruit", Green, 2, 1, 0, "----L-", 0, 0, 0, "Give a friendly creature +1/+0 and Lethal. " },
{ 121, "Enchanted Hat", Green, 2, 0, 3, "------", 0, 0, 1, "Give a friendly creature +0/+3.\nDraw a card. " },
{ 122, "Bolstering Bread", Green, 2, 1, 3, "---G--", 0, 0, 0, "Give a friendly creature +1/+3 and Guard. " },
{ 123, "Wristguards", Green, 2, 4, 0, "------", 0, 0, 0, "Give a friendly creature +4/+0. " },
{ 124, "Blood Grapes", Green, 3, 2, 1, "--D---", 0, 0, 0, "Give a friendly creature +2/+1 and Drain. " },
{ 125, "Healthy Veggies", Green, 3, 1, 4, "------", 0, 0, 0, "Give a friendly creature +1/+4. " },
{ 126, "Heavy Shield", Green, 3, 2, 3, "------", 0, 0, 0, "Give a friendly creature +2/+3. " },
{ 127, "Imperial Helm", Green, 3, 0, 6, "------", 0, 0, 0, "Give a friendly creature +0/+6. " },
{ 128, "Enchanted Cloth", Green, 4, 4, 3, "------", 0, 0, 0, "Give a friendly creature +4/+3. " },
{ 129, "Enchanted Leather", Green, 4, 2, 5, "------", 0, 0, 0, "Give a friendly creature +2/+5. " },
{ 130, "Helm of Remedy", Green, 4, 0, 6, "------", 4, 0, 0, "Give a friendly creature +0/+6.\nYou gain 4 health. " },
{ 131, "Heavy Gauntlet", Green, 4, 4, 1, "------", 0, 0, 0, "Give a friendly creature +4/+1. " },
{ 132, "High Protein", Green, 5, 3, 3, "B-----", 0, 0, 0, "Give a friendly creature +3/+3 and Breakthrough. " },
{ 133, "Pie of Power", Green, 5, 4, 0, "-----W", 0, 0, 0, "Give a friendly creature +4/+0 and Ward. " },
{ 134, "Light The Way", Green, 4, 2, 2, "------", 0, 0, 1, "Give a friendly creature +2/+2.\nDraw a card. " },
{ 135, "Imperial Armour", Green, 6, 5, 5, "------", 0, 0, 0, "Give a friendly creature +5/+5. " },
{ 136, "Buckler", Green, 0, 1, 1, "------", 0, 0, 0, "Give a friendly creature +1/+1. " },
{ 137, "Ward", Green, 2, 0, 0, "-----W", 0, 0, 0, "Give a friendly creature Ward. " },
{ 138, "Grow Horns", Green, 2, 0, 0, "---G--", 0, 0, 1, "Give a friendly creature Guard.\nDraw a card. " },
{ 139, "Grow Stingers", Green, 4, 0, 0, "----LW", 0, 0, 0, "Give a friendly creature Lethal and Ward. " },
{ 140, "Grow Wings", Green, 2, 0, 0, "-C----", 0, 0, 0, "Give a friendly creature Charge. " },
{ 141, "Throwing Knife", Red, 0, -1, -1, "------", 0, 0, 0, "Give an enemy creature -1/-1. " },
{ 142, "Staff of Suppression", Red, 0, 0, 0, "BCDGLW", 0, 0, 0, "Remove all abilities from an enemy creature. " },
{ 143, "Pierce Armour", Red, 0, 0, 0, "---G--", 0, 0, 0, "Remove Guard from an enemy creature. " },
{ 144, "Rune Axe", Red, 1, 0, -2, "------", 0, 0, 0, "Deal 2 damage to an enemy creature. " },
{ 145, "Cursed Sword", Red, 3, -2, -2, "------", 0, 0, 0, "Give an enemy creature -2/-2. " },
{ 146, "Cursed Scimitar", Red, 4, -2, -2, "------", 0, -2, 0, "Give an enemy creature -2/-2.\nDeal 2 damage to your opponent. " },
{ 147, "Quick Shot", Red, 2, 0, -1, "------", 0, 0, 1, "Deal 1 damage to an enemy creature.\nDraw a card. " },
{ 148, "Helm Crusher", Red, 2, 0, -2, "BCDGLW", 0, 0, 0, "Remove all abilities from an enemy creature,\nthen deal 2 damage to it. " },
{ 149, "Rootkin Ritual", Red, 3, 0, 0, "BCDGLW", 0, 0, 1, "Remove all abilities from an enemy creature.\nDraw a card. " },
{ 150, "Throwing Axe", Red, 2, 0, -3, "------", 0, 0, 0, "Deal 3 damage to an enemy creature. " },
{ 151, "Decimate", Red, 5, 0, -99, "BCDGLW", 0, 0, 0, "Remove all abilities from an enemy creature,\nthen deal 99 damage to it. " },
{ 152, "Mighty Throwing Axe", Red, 7, 0, -7, "------", 0, 0, 1, "Deal 7 damage to an enemy creature.\nDraw a card. " },
{ 153, "Healing Potion", Blue, 2, 0, 0, "------", 5, 0, 0, "Gain 5 health. " },
{ 154, "Poison", Blue, 2, 0, 0, "------", 0, -2, 1, "Deal 2 damage to your opponent.\nDraw a card." },
{ 155, "Scroll of Firebolt", Blue, 3, 0, -3, "------", 0, -1, 0, "Deal 3 damage.\nDeal 1 damage to your opponent " },
{ 156, "Major Life Steal Potion", Blue, 3, 0, 0, "------", 3, -3, 0, "Deal 3 damage to your opponent and gain 3 health. " },
{ 157, "Life Sap Drop", Blue, 3, 0, -1, "------", 1, 0, 1, "Deal 1 damage, gain 1 health, and draw a card. " },
{ 158, "Tome of Thunder", Blue, 3, 0, -4, "------", 0, 0, 0, "Deal 4 damage. " },
{ 159, "Vial of Soul Drain", Blue, 4, 0, -3, "------", 3, 0, 0, "Deal 3 damage and gain 3 health. " },
{ 160, "Minor Life Steal Potion", Blue, 2, 0, 0, "------", 2, -2, 0, "Deal 2 damage to your opponent and gain 2 health. " },
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

template <typename C>
const Card* get_card(const C& cards, int id)
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

    void start_of_turn()
    {
        for (Card& card : myBoard)
            card.new_this_turn = false;
    }

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
            if (card->cardType == Red || card->defense < 0)
            {
                const Card* opCard = get_card(opBoard, action.b);
                if (!opCard)
                    return false;
            }
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
            card->new_this_turn = true;
            myBoard.push_back(*card);
        }
        break;

        case Action::ATTACK:
        {
            card->used = true;
            card->new_this_turn = false;
            if (action.b == -1)
            {
                op.health -= card->attack;
                update_runes();
                if (card->drain)
                    me.health += card->attack;
                return nullptr;
            }

            Card* opCard = get_card(opBoard, action.b);
            if (!opCard)
                return "opponent attack target not found";

            bool me_blocked = opCard->ward;
            bool op_blocked = card->ward;
            if (card->attack > 0)
                opCard->ward = false;
            if (opCard->attack > 0)
                card->ward = false;

            if (!op_blocked)
            {
                card->defense -= opCard->attack;
                if (opCard->lethal)
                    card->defense = 0;
            }

            if (!me_blocked)
            {
                int damage = min(card->attack, opCard->defense);
                opCard->defense -= damage;

                if (card->lethal)
                    opCard->defense = 0;

                if (card->breakthrough)
                {
                    op.health -= card->attack - damage;
                    update_runes();
                }

                if (card->drain)
                    me.health += card->attack;
            }

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
            update_runes();
            me.mana -= card->cost;
            me.draw += card->cardDraw;
            remove_card(*card, myHand);
        }

        return nullptr;
    }

    void update_runes()
    {
        while (op.health <= op.rune)
        {
            op.rune -= 5;
            op.draw += 1;
        }
        while (me.health <= me.rune)
        {
            me.rune -= 5;
            me.draw += 1;
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
        if (effect.charge)
        {
            card.charge = effect.charge;
            if (card.new_this_turn)
                card.used = false;
        }
        card.drain = card.drain || effect.drain;
        card.guard = card.guard || effect.guard;
        card.lethal = card.lethal || effect.lethal;
        card.ward = card.ward || effect.ward;
    }

    void red_effect(const Card& effect, Card& card)
    {
        card.breakthrough = card.breakthrough && !effect.breakthrough;
        card.charge = card.charge && !effect.charge;
        card.drain = card.drain && !effect.drain;
        card.guard = card.guard && !effect.guard;
        card.lethal = card.lethal && !effect.lethal;
        card.ward = card.ward && !effect.ward;
        if (card.ward)
        {
            if (card.attack != 0 || card.defense != 0)
                card.ward = false;
        }
        else
        {
            card.attack += effect.attack;
            card.defense += effect.defense;
        }
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

#ifdef DEVELOPMENT
extern vector<pair<int, Action>> op_actions;
#else
vector<pair<int, Action>> op_actions;
#endif

inline istream& operator>>(istream& in, VisibleState& state)
{
    in >> state.me;
    in >> state.op;

    int opponentHand;
    int opponentActions;
    cin >> opponentHand >> opponentActions; cin.ignore();
    op_actions.clear();
    for (int i = 0; i < opponentActions; i++) {
        string cardNumberAndAction;
        getline(cin, cardNumberAndAction);
        int card=0;
        string action;
        int a=0, b=0;
        stringstream in(cardNumberAndAction);
        in >> card >> action >> a >> b;
        if (card > 0)
        {
            Action::What what;
            if (action == "PICK") what = Action::PICK;
            if (action == "SUMMON") what = Action::SUMMON;
            if (action == "ATTACK") what = Action::ATTACK;
            if (action == "USE") what = Action::USE;
            if (action == "PASS") what = Action::PASS;
            op_actions.push_back({ card, {what, a, b} });
        }
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

struct IntParams
{
    int curve[13] = { 0, 5, 4, 3, 3, 3, 0, 0 };
};

struct DoubleParams
{
    double attack_m = 1;
    double defense_m = 1;
    double mult_m = 1;
    double cost_a = 1;
    double cost_pow = -1;
    double flex_min = 0.0001;
    double flex_pen = 1;
    double attr_b = 0;

    double attr_c = 0;
    double attr_d = 0;
    double attr_g = 0;
    double attr_l = 0;
    double attr_w = 0;
    double attr_u = 0;
    double creature = 0;
    double attr_gb = 0;

    double attr_gd = 0;
    double attr_gl = 0;
    double attr_gw = 0;
    double health = 1;
    double my_hand = 0.5;
    double my_board = 1;
    double op_board = 1;
    double num_board = 0;

    double attr_op_b = 0;
    double attr_op_c = 0;
    double attr_op_d = 0;
    double attr_op_g = 0;
    double attr_op_l = 0;
    double attr_op_w = 0;
    double draft_attack_m = 1;
    double draft_defense_m = 1;

    double draft_mult_m = 1;
    double draft_attr_b = 0;
    double draft_attr_c = 0;
    double draft_attr_d = 0;
    double draft_attr_g = 0;
    double draft_attr_l = 0;
    double draft_attr_w = 0;
    double draw_30 = 0;

    double draw_0 = 0;
    double draft_item_b = 0;
    double draft_item_c = 0;
    double draft_item_d = 0;
    double draft_item_g = 0;
    double draft_item_l = 0;
    double draft_item_w = 0;
    double draft_add_my_health = 1;

    double draft_sub_op_health = 1;
    double draft_draw = 1;
    double draft_item_attack_m = 0;
    double draft_item_defense_m = 0;
    double draft_item_mult_m = 0;
    double itemRed = 0;
    double itemGreen = 0;
    double itemBlue = 0;

    double maxDamage = 7;
    double card_m_a = 0;
    double card_m_d = 0;
    double item_m_a = 0;
    double item_m_d = 0;
};

struct Params
{
    IntParams i;
    DoubleParams d;
};

class Player
{
public:
    Deck deck;
    vector<int> cost_counts;
    const Params v;

    Player(Params params = Params())
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

    virtual Action draftAction(const VisibleState& state)
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

    vector<Action> get_item_actions(const Card& card, const VisibleState& state)
    {
        vector<Action> actions;
        switch (card.cardType)
        {
        case Creature:
        {
            break;
        }
        case Green:
        {
            for (const Card& myCard : state.myBoard)
            {
                Action use = { Action::USE, card.instanceId, myCard.instanceId };
                if (state.can(use))
                    actions.push_back(use);
            }
            break;
        }
        case Blue:
            if (card.defense == 0)
            {
                Action use = { Action::USE, card.instanceId, -1 };
                if (state.can(use))
                    actions.push_back(use);
                break;
            }
            // else deliberate fall through
        case Red:
        {
            for (const Card& opCard : state.opBoard)
            {
                Action use = { Action::USE, card.instanceId, opCard.instanceId };
                if (state.can(use))
                    actions.push_back(use);
            }
            break;
        }
        }
        return actions;
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

    vector<vector<Action>> find_best_spends(const VisibleState& in_state)
    {
        // selection order is arranged so that the most expensive selections are considered first
        HandCards hand = in_state.myHand;
        sort(hand.begin(), hand.end(), [](const Card& a, const Card& b) {return a.cost < b.cost; });

        vector<vector<Action>> actions;

        // for each subset of cards
        size_t n = hand.size();
        int n_subsets = 1 << n;
        for (int subset_bits = n_subsets - 1; subset_bits >= 0; subset_bits--)
        {
            HandCards creatures, items;
            int spent = 0;

            // can we afford?
            for (size_t i = 0; i < n; i++)
            {
                if (subset_bits & (1 << i))
                {
                    const Card& card = hand[i];
                    if (card.cardType == Creature)
                        creatures.push_back(card);
                    else
                        items.push_back(card);
                    spent += card.cost;
                }
            }
            if (spent > in_state.me.mana)
                continue;

            StateChange change = { in_state };

            // summon creatures
            bool all_done = true;
            for (const Card& creature : creatures)
            {
                Action summon = { Action::SUMMON, creature.instanceId };
                all_done = all_done && change.state.can(summon);
                if (!all_done)
                    break;
                change.add(summon);
            }
            if (!all_done)
                continue;

            // for each use combination
            vector<vector<Action>> item_actions;
            vector<size_t> selection;
            size_t n_selections = 1;
            size_t n_items = items.size();
            for (const Card& item : items)
            {
                vector<Action> actions = get_item_actions(item, change.state);
                item_actions.push_back(actions);
                selection.push_back(0);
                n_selections *= actions.size();
            }

            for (size_t s = 0; s < n_selections; s++)
            {
                StateChange item_change = change;

                // do actions
                all_done = true;
                for (size_t i = 0; i < n_items; i++)
                {
                    Action use = item_actions[i][selection[i]];
                    all_done = all_done && item_change.state.can(use);
                    if (!all_done)
                        break;
                    item_change.add(use);
                }
                if (!all_done)
                    continue;

                actions.push_back(item_change.actions);
                if (actions.size() >= 1000)
                    return actions;

                // increment selection
                for (size_t i = 0; i < n_items; i++)
                {
                    size_t& s = selection[i];
                    s++;
                    if (s == item_actions[i].size())
                        s = 0;
                    else
                        break;
                }
            }
        }

        return actions;
    }

    std::chrono::time_point<std::chrono::system_clock> start_time;
    int elapsed()
    {
        return int(std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - start_time).count());
    }
    const int timeout = 80;

    virtual vector<Action> battleAction(const VisibleState& in_state)
    {
        start_time = std::chrono::system_clock::now();
        StateChange change{ in_state };

        while (change.state.me.mana > 0 && elapsed() < timeout)
        {
            vector<Action> next_actions = battleStep(change.state);
            if (next_actions.empty())
                break;
            change.add(next_actions);
        }

        if (change.actions.empty())
            change.actions.push_back({ Action::PASS });

        return change.actions;
    }

    vector<Action> battleStep(const VisibleState& in_state)
    {
        vector<Action> best_actions;
        double best_score = bad_score;

        // split guards and other creatures
        BoardCards opGuard;
        BoardCards opCreature;

        for (const Card& card : in_state.opBoard)
        {
            if (card.guard)
                opGuard.push_back(card);
            else
                opCreature.push_back(card);
        }

        // find ways to spend mana
        vector<vector<Action>> spends = find_best_spends(in_state);
        for (const vector<Action>& spend : spends)
        {
            if (elapsed() > timeout)
                break;

            StateChange change{ in_state };
            change.add(spend);

            vector<Action> guard_kills = kill_guards(change.state, opGuard);
            change.add(guard_kills);

            vector<Action> attacks = best_attacks(change.state, opCreature);
            change.add(attacks);

            double score = score_state(change.state);
            if (score > best_score)
            {
                best_score = score;
                best_actions = change.actions;
            }
        }

        return best_actions;
    }

    vector<Action> kill_guards(const VisibleState& in_state, BoardCards opGuard)
    {
        StateChange change { in_state };
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
        return change.actions;
    }

    vector<Action> best_attacks(const VisibleState& in_state, BoardCards opCreature)
    {
        // split the remaining attacks
        double best_score = bad_score;
        vector<Action> best_attacks;
        vector<Action> attacks;
        for (;;)
        {
            // try the current attack actions, then face hit
            StateChange attack_change{ in_state };
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

        return best_attacks;
    }
};

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

struct FullState
{
    bool draw_phase;
    PlayerState player_states[2];
    HandCards hands[2];
    BoardCards boards[2];
    Deck decks[2];
    MultiPlayerRules rules[2] =
    {
        {draw_phase, player_states[0], player_states[1], hands[0], boards[0], boards[1]},
        {draw_phase, player_states[1], player_states[0], hands[1], boards[1], boards[0]}
    };
};

inline string boards_diff(const BoardCards& a, const BoardCards& b, string who)
{
    if (a.size() != b.size())
    {
        stringstream strm; strm << who << a.size() << " size != " << b.size(); return strm.str();
    }
    for (const Card& c : a)
    {
        const Card* d = get_card(b, c.instanceId);
        if (!d)
        {
            stringstream strm; strm << who << " missing card from b " << c.instanceId; return strm.str();
        }
        string diff = c.diff(*d);
        if (!diff.empty())
            return who + diff;
    }
    return "";
}

const Params current_best_params =
{ {
0, 2, 4, 5, 2, 1, -5, -7,
-5, -11, -8, -7, -7,
},
{
2.80787, 1.9218, 0.261229, 0.725032, -1.28739, 0.830066, 0.567213, -0.414187,
1.45776, -0.272006, 0.575817, -0.25395, 2.73163, -0.313109, 3.85514, 0.872998,
1.56796, 1.64741, 1.14193, -0.157035, 0.0580965, 1.1822, 1.53493, 0,
0.638911, -1.02047, -0.346689, -0.762542, 19.8183, -1.02183, 1.14937, 0.560557,
2.94441, -4.74371, -3.15498, -0.573178, 0.0780187, -0.0281649, 3.60422, -0.00324518,
3.98431, -1.02113, -1.79464, 1.22999, 0.568018, 1.63991, -0.468817, 0.186743,
6.42454, 14.9243, -0.0170047, 6.03354, -2.36409, -2.46361, -0.550072, -0.497079,
25.1896, 0, 0.0723614, -0.216477, -0.0144239,
} };

inline void codingame_loop()
{
    Player player(current_best_params);

    string message = "";
    FullState prevState;
    prevState.draw_phase = false;

    // game loop
    while (1) {
        VisibleState vs;
        cin >> vs;
        vs.start_of_turn();

        for (auto op_action : op_actions)
        {
            prevState.player_states[1].mana = 99;
            if (op_action.second.what == Action::SUMMON || op_action.second.what == Action::USE)
            {
                Card card = card_info[op_action.first - 1].to_card();
                card.instanceId = op_action.second.a;
                prevState.hands[1].push_back(card);
            }
            prevState.rules[1].act(op_action.second);
        }
        if (vs.me.mana > 0)
        {
            string new_message = boards_diff(prevState.boards[0], vs.myBoard, "my") +
                boards_diff(prevState.boards[1], vs.opBoard, "op") +
                prevState.player_states[0].diff(vs.me) +
                prevState.player_states[1].diff(vs.op);
            if (!new_message.empty())
                message = new_message;
        }

        vector<Action> actions;
        if (vs.draw_phase)
            actions.push_back(player.draftAction(vs));
        else
            actions = player.battleAction(vs);

        prevState.boards[0] = vs.myBoard;
        prevState.boards[1] = vs.opBoard;
        prevState.hands[0] = vs.myHand;
        prevState.player_states[0] = vs.me;
        prevState.player_states[1] = vs.op;

        bool first = true;
        for (const Action& action : actions)
        {
            if (!first)
                cout << ";";
            first = false;
            cout << action;
            if (!message.empty())
                cout << " " << message;
            prevState.rules[0].act(action);
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
