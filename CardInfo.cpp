#include "CardInfo.h"
#include <random>

namespace
{
    int creature = Creature;
    int itemRed = Red;
    int itemGreen = Green;
    int itemBlue = Blue;
}

static const CardInfo card_info[160] =
{
{ 1, "Slimer", creature, 1, 2, 1, "------", 1, 0, 0, "Summon: You gain 1 health. " },
{ 2, "Scuttler", creature, 1, 1, 2, "------", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 3, "Beavrat", creature, 1, 2, 2, "------", 0, 0, 0, "" },
{ 4, "Plated Toad", creature, 2, 1, 5, "------", 0, 0, 0, "" },
{ 5, "Grime Gnasher", creature, 2, 4, 1, "------", 0, 0, 0, "" },
{ 6, "Murgling", creature, 2, 3, 2, "------", 0, 0, 0, "" },
{ 7, "Rootkin Sapling", creature, 2, 2, 2, "-----W", 0, 0, 0, "" },
{ 8, "Psyshroom", creature, 2, 2, 3, "------", 0, 0, 0, "" },
{ 9, "Corrupted Beavrat", creature, 3, 3, 4, "------", 0, 0, 0, "" },
{ 10, "Carnivorous Bush", creature, 3, 3, 1, "--D---", 0, 0, 0, "" },
{ 11, "Snowsaur", creature, 3, 5, 2, "------", 0, 0, 0, "" },
{ 12, "Woodshroom", creature, 3, 2, 5, "------", 0, 0, 0, "" },
{ 13, "Swamp Terror", creature, 4, 5, 3, "------", 1, -1, 0, "Summon: You gain 1 health and deal\n1 damage to your opponent. " },
{ 14, "Fanged Lunger", creature, 4, 9, 1, "------", 0, 0, 0, "" },
{ 15, "Pouncing Flailmouth", creature, 4, 4, 5, "------", 0, 0, 0, "" },
{ 16, "Wrangler Fish", creature, 4, 6, 2, "------", 0, 0, 0, "" },
{ 17, "Ash Walker", creature, 4, 4, 5, "------", 0, 0, 0, "" },
{ 18, "Acid Golem", creature, 4, 7, 4, "------", 0, 0, 0, "" },
{ 19, "Foulbeast", creature, 5, 5, 6, "------", 0, 0, 0, "" },
{ 20, "Hedge Demon", creature, 5, 8, 2, "------", 0, 0, 0, "" },
{ 21, "Crested Scuttler", creature, 5, 6, 5, "------", 0, 0, 0, "" },
{ 22, "Sigbovak", creature, 6, 7, 5, "------", 0, 0, 0, "" },
{ 23, "Titan Cave Hog", creature, 7, 8, 8, "------", 0, 0, 0, "" },
{ 24, "Exploding Skitterbug", creature, 1, 1, 1, "------", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 25, "Spiney Chompleaf", creature, 2, 3, 1, "------", -2, -2, 0, "Summon: Deal 2 damage to each player. " },
{ 26, "Razor Crab", creature, 2, 3, 2, "------", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 27, "Nut Gatherer", creature, 2, 2, 2, "------", 2, 0, 0, "Summon: You gain 2 health. " },
{ 28, "Infested Toad", creature, 2, 1, 2, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 29, "Steelplume Nestling", creature, 2, 2, 1, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 30, "Venomous Bog Hopper", creature, 3, 4, 2, "------", 0, -2, 0, "Summon: Deal 2 damage to your opponent. " },
{ 31, "Woodland Hunter", creature, 3, 3, 1, "------", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 32, "Sandsplat", creature, 3, 3, 2, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 33, "Chameleskulk", creature, 4, 4, 3, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 34, "Eldritch Cyclops", creature, 5, 3, 5, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 35, "Snail-eyed Hulker", creature, 6, 5, 2, "B-----", 0, 0, 1, "Summon: Draw a card. " },
{ 36, "Possessed Skull", creature, 6, 4, 4, "------", 0, 0, 2, "Summon: Draw two cards. " },
{ 37, "Eldritch Multiclops", creature, 6, 5, 7, "------", 0, 0, 1, "Summon: Draw a card. " },
{ 38, "Imp", creature, 1, 1, 3, "--D---", 0, 0, 0, "" },
{ 39, "Voracious Imp", creature, 1, 2, 1, "--D---", 0, 0, 0, "" },
{ 40, "Rock Gobbler", creature, 3, 2, 3, "--DG--", 0, 0, 0, "" },
{ 41, "Blizzard Demon", creature, 3, 2, 2, "-CD---", 0, 0, 0, "" },
{ 42, "Flying Leech", creature, 4, 4, 2, "--D---", 0, 0, 0, "" },
{ 43, "Screeching Nightmare", creature, 6, 5, 5, "--D---", 0, 0, 0, "" },
{ 44, "Deathstalker", creature, 6, 3, 7, "--D-L-", 0, 0, 0, "" },
{ 45, "Night Howler", creature, 6, 6, 5, "B-D---", -3, 0, 0, "Summon: You lose 3 health." },
{ 46, "Soul Devourer", creature, 9, 7, 7, "--D---", 0, 0, 0, "" },
{ 47, "Gnipper", creature, 2, 1, 5, "--D---", 0, 0, 0, "" },
{ 48, "Venom Hedgehog", creature, 1, 1, 1, "----L-", 0, 0, 0, "" },
{ 49, "Shiny Prowler", creature, 2, 1, 2, "---GL-", 0, 0, 0, "" },
{ 50, "Puff Biter", creature, 3, 3, 2, "----L-", 0, 0, 0, "" },
{ 51, "Elite Bilespitter", creature, 4, 3, 5, "----L-", 0, 0, 0, "" },
{ 52, "Bilespitter", creature, 4, 2, 4, "----L-", 0, 0, 0, "" },
{ 53, "Possessed Abomination", creature, 4, 1, 1, "-C--L-", 0, 0, 0, "" },
{ 54, "Shadow Biter", creature, 3, 2, 2, "----L-", 0, 0, 0, "" },
{ 55, "Hermit Slime", creature, 2, 0, 5, "---G--", 0, 0, 0, "" },
{ 56, "Giant Louse", creature, 4, 2, 7, "------", 0, 0, 0, "" },
{ 57, "Dream-Eater", creature, 4, 1, 8, "------", 0, 0, 0, "" },
{ 58, "Darkscale Predator", creature, 6, 5, 6, "B-----", 0, 0, 0, "" },
{ 59, "Sea Ghost", creature, 7, 7, 7, "------", 1, -1, 0, "Summon: You gain 1 health and deal\n1 damage to your opponent. " },
{ 60, "Gritsuck Troll", creature, 7, 4, 8, "------", 0, 0, 0, "" },
{ 61, "Alpha Troll", creature, 9, 10, 10, "------", 0, 0, 0, "" },
{ 62, "Mutant Troll", creature, 12, 12, 12, "B--G--", 0, 0, 0, "" },
{ 63, "Rootkin Drone", creature, 2, 0, 4, "---G-W", 0, 0, 0, "" },
{ 64, "Coppershell Tortoise", creature, 2, 1, 1, "---G-W", 0, 0, 0, "" },
{ 65, "Steelplume Defender", creature, 2, 2, 2, "-----W", 0, 0, 0, "" },
{ 66, "Staring Wickerbeast", creature, 5, 5, 1, "-----W", 0, 0, 0, "" },
{ 67, "Flailing Hammerhead", creature, 6, 5, 5, "-----W", 0, -2, 0, "Summon: Deal 2 damage to your opponent. " },
{ 68, "Giant Squid", creature, 6, 7, 5, "-----W", 0, 0, 0, "" },
{ 69, "Charging Boarhound", creature, 3, 4, 4, "B-----", 0, 0, 0, "" },
{ 70, "Murglord", creature, 4, 6, 3, "B-----", 0, 0, 0, "" },
{ 71, "Flying Murgling", creature, 4, 3, 2, "BC----", 0, 0, 0, "" },
{ 72, "Shuffling Nightmare", creature, 4, 5, 3, "B-----", 0, 0, 0, "" },
{ 73, "Bog Bounder", creature, 4, 4, 4, "B-----", 4, 0, 0, "Summon: You gain 4 health. " },
{ 74, "Crusher", creature, 5, 5, 4, "B--G--", 0, 0, 0, "" },
{ 75, "Titan Prowler", creature, 5, 6, 5, "B-----", 0, 0, 0, "" },
{ 76, "Crested Chomper", creature, 6, 5, 5, "B-D---", 0, 0, 0, "" },
{ 77, "Lumbering Giant", creature, 7, 7, 7, "B-----", 0, 0, 0, "" },
{ 78, "Shambler", creature, 8, 5, 5, "B-----", 0, -5, 0, "Summon: Deal 5 damage to your opponent. " },
{ 79, "Scarlet Colossus", creature, 8, 8, 8, "B-----", 0, 0, 0, "" },
{ 80, "Corpse Guzzler", creature, 8, 8, 8, "B--G--", 0, 0, 1, "Summon: Draw a card. " },
{ 81, "Flying Corpse Guzzler", creature, 9, 6, 6, "BC----", 0, 0, 0, "" },
{ 82, "Slithering Nightmare", creature, 7, 5, 5, "B-D--W", 0, 0, 0, "" },
{ 83, "Restless Owl", creature, 0, 1, 1, "-C----", 0, 0, 0, "" },
{ 84, "Fighter Tick", creature, 2, 1, 1, "-CD--W", 0, 0, 0, "" },
{ 85, "Heartless Crow", creature, 3, 2, 3, "-C----", 0, 0, 0, "" },
{ 86, "Crazed Nose-pincher", creature, 3, 1, 5, "-C----", 0, 0, 0, "" },
{ 87, "Bloat Demon", creature, 4, 2, 5, "-C-G--", 0, 0, 0, "" },
{ 88, "Abyss Nightmare", creature, 5, 4, 4, "-C----", 0, 0, 0, "" },
{ 89, "Boombeak", creature, 5, 4, 1, "-C----", 2, 0, 0, "Summon: You gain 2 health." },
{ 90, "Eldritch Swooper", creature, 8, 5, 5, "-C----", 0, 0, 0, "" },
{ 91, "Flumpy", creature, 0, 1, 2, "---G--", 0, 1, 0, "Summon: Your opponent gains 1 health." },
{ 92, "Wurm", creature, 1, 0, 1, "---G--", 2, 0, 0, "Summon: You gain 2 health. " },
{ 93, "Spinekid", creature, 1, 2, 1, "---G--", 0, 0, 0, "" },
{ 94, "Rootkin Defender", creature, 2, 1, 4, "---G--", 0, 0, 0, "" },
{ 95, "Wildum", creature, 2, 2, 3, "---G--", 0, 0, 0, "" },
{ 96, "Prairie Protector", creature, 2, 3, 2, "---G--", 0, 0, 0, "" },
{ 97, "Turta", creature, 3, 3, 3, "---G--", 0, 0, 0, "" },
{ 98, "Lilly Hopper", creature, 3, 2, 4, "---G--", 0, 0, 0, "" },
{ 99, "Cave Crab", creature, 3, 2, 5, "---G--", 0, 0, 0, "" },
{ 100, "Stalagopod", creature, 3, 1, 6, "---G--", 0, 0, 0, "" },
{ 101, "Engulfer", creature, 4, 3, 4, "---G--", 0, 0, 0, "" },
{ 102, "Mole Demon", creature, 4, 3, 3, "---G--", 0, -1, 0, "Summon: Deal 1 damage to your opponent. " },
{ 103, "Mutating Rootkin", creature, 4, 3, 6, "---G--", 0, 0, 0, "" },
{ 104, "Deepwater Shellcrab", creature, 4, 4, 4, "---G--", 0, 0, 0, "" },
{ 105, "King Shellcrab", creature, 5, 4, 6, "---G--", 0, 0, 0, "" },
{ 106, "Far-reaching Nightmare", creature, 5, 5, 5, "---G--", 0, 0, 0, "" },
{ 107, "Worker Shellcrab", creature, 5, 3, 3, "---G--", 3, 0, 0, "Summon: You gain 3 health. " },
{ 108, "Rootkin Elder", creature, 5, 2, 6, "---G--", 0, 0, 0, "" },
{ 109, "Elder Engulfer", creature, 5, 5, 6, "------", 0, 0, 0, "" },
{ 110, "Gargoyle", creature, 5, 0, 9, "---G--", 0, 0, 0, "" },
{ 111, "Turta Knight", creature, 6, 6, 6, "---G--", 0, 0, 0, "" },
{ 112, "Rootkin Leader", creature, 6, 4, 7, "---G--", 0, 0, 0, "" },
{ 113, "Tamed Bilespitter", creature, 6, 2, 4, "---G--", 4, 0, 0, "Summon: You gain 4 health. " },
{ 114, "Gargantua", creature, 7, 7, 7, "---G--", 0, 0, 0, "" },
{ 115, "Rootkin Warchief", creature, 8, 5, 5, "---G-W", 0, 0, 0, "" },
{ 116, "Emperor Nightmare", creature, 12, 8, 8, "BCDGLW", 0, 0, 0, "" },
{ 117, "Protein", itemGreen, 1, 1, 1, "B-----", 0, 0, 0, "Give a friendly creature +1/+1 and Breakthrough. " },
{ 118, "Royal Helm", itemGreen, 0, 0, 3, "------", 0, 0, 0, "Give a friendly creature +0/+3. " },
{ 119, "Serrated Shield", itemGreen, 1, 1, 2, "------", 0, 0, 0, "Give a friendly creature +1/+2. " },
{ 120, "Venomfruit", itemGreen, 2, 1, 0, "----L-", 0, 0, 0, "Give a friendly creature +1/+0 and Lethal. " },
{ 121, "Enchanted Hat", itemGreen, 2, 0, 3, "------", 0, 0, 1, "Give a friendly creature +0/+3.\nDraw a card. " },
{ 122, "Bolstering Bread", itemGreen, 2, 1, 3, "---G--", 0, 0, 0, "Give a friendly creature +1/+3 and Guard. " },
{ 123, "Wristguards", itemGreen, 2, 4, 0, "------", 0, 0, 0, "Give a friendly creature +4/+0. " },
{ 124, "Blood Grapes", itemGreen, 3, 2, 1, "--D---", 0, 0, 0, "Give a friendly creature +2/+1 and Drain. " },
{ 125, "Healthy Veggies", itemGreen, 3, 1, 4, "------", 0, 0, 0, "Give a friendly creature +1/+4. " },
{ 126, "Heavy Shield", itemGreen, 3, 2, 3, "------", 0, 0, 0, "Give a friendly creature +2/+3. " },
{ 127, "Imperial Helm", itemGreen, 3, 0, 6, "------", 0, 0, 0, "Give a friendly creature +0/+6. " },
{ 128, "Enchanted Cloth", itemGreen, 4, 4, 3, "------", 0, 0, 0, "Give a friendly creature +4/+3. " },
{ 129, "Enchanted Leather", itemGreen, 4, 2, 5, "------", 0, 0, 0, "Give a friendly creature +2/+5. " },
{ 130, "Helm of Remedy", itemGreen, 4, 0, 6, "------", 4, 0, 0, "Give a friendly creature +0/+6.\nYou gain 4 health. " },
{ 131, "Heavy Gauntlet", itemGreen, 4, 4, 1, "------", 0, 0, 0, "Give a friendly creature +4/+1. " },
{ 132, "High Protein", itemGreen, 5, 3, 3, "B-----", 0, 0, 0, "Give a friendly creature +3/+3 and Breakthrough. " },
{ 133, "Pie of Power", itemGreen, 5, 4, 0, "-----W", 0, 0, 0, "Give a friendly creature +4/+0 and Ward. " },
{ 134, "Light The Way", itemGreen, 4, 2, 2, "------", 0, 0, 1, "Give a friendly creature +2/+2.\nDraw a card. " },
{ 135, "Imperial Armour", itemGreen, 6, 5, 5, "------", 0, 0, 0, "Give a friendly creature +5/+5. " },
{ 136, "Buckler", itemGreen, 0, 1, 1, "------", 0, 0, 0, "Give a friendly creature +1/+1. " },
{ 137, "Ward", itemGreen, 2, 0, 0, "-----W", 0, 0, 0, "Give a friendly creature Ward. " },
{ 138, "Grow Horns", itemGreen, 2, 0, 0, "---G--", 0, 0, 1, "Give a friendly creature Guard.\nDraw a card. " },
{ 139, "Grow Stingers", itemGreen, 4, 0, 0, "----LW", 0, 0, 0, "Give a friendly creature Lethal and Ward. " },
{ 140, "Grow Wings", itemGreen, 2, 0, 0, "-C----", 0, 0, 0, "Give a friendly creature Charge. " },
{ 141, "Throwing Knife", itemRed, 0, -1, -1, "------", 0, 0, 0, "Give an enemy creature -1/-1. " },
{ 142, "Staff of Suppression", itemRed, 0, 0, 0, "BCDGLW", 0, 0, 0, "Remove all abilities from an enemy creature. " },
{ 143, "Pierce Armour", itemRed, 0, 0, 0, "---G--", 0, 0, 0, "Remove Guard from an enemy creature. " },
{ 144, "Rune Axe", itemRed, 1, 0, -2, "------", 0, 0, 0, "Deal 2 damage to an enemy creature. " },
{ 145, "Cursed Sword", itemRed, 3, -2, -2, "------", 0, 0, 0, "Give an enemy creature -2/-2. " },
{ 146, "Cursed Scimitar", itemRed, 4, -2, -2, "------", 0, -2, 0, "Give an enemy creature -2/-2.\nDeal 2 damage to your opponent. " },
{ 147, "Quick Shot", itemRed, 2, 0, -1, "------", 0, 0, 1, "Deal 1 damage to an enemy creature.\nDraw a card. " },
{ 148, "Helm Crusher", itemRed, 2, 0, -2, "BCDGLW", 0, 0, 0, "Remove all abilities from an enemy creature,\nthen deal 2 damage to it. " },
{ 149, "Rootkin Ritual", itemRed, 3, 0, 0, "BCDGLW", 0, 0, 1, "Remove all abilities from an enemy creature.\nDraw a card. " },
{ 150, "Throwing Axe", itemRed, 2, 0, -3, "------", 0, 0, 0, "Deal 3 damage to an enemy creature. " },
{ 151, "Decimate", itemRed, 5, 0, -99, "BCDGLW", 0, 0, 0, "Remove all abilities from an enemy creature,\nthen deal 99 damage to it. " },
{ 152, "Mighty Throwing Axe", itemRed, 7, 0, -7, "------", 0, 0, 1, "Deal 7 damage to an enemy creature.\nDraw a card. " },
{ 153, "Healing Potion", itemBlue, 2, 0, 0, "------", 5, 0, 0, "Gain 5 health. " },
{ 154, "Poison", itemBlue, 2, 0, 0, "------", 0, -2, 1, "Deal 2 damage to your opponent.\nDraw a card." },
{ 155, "Scroll of Firebolt", itemBlue, 3, 0, -3, "------", 0, -1, 0, "Deal 3 damage.\nDeal 1 damage to your opponent " },
{ 156, "Major Life Steal Potion", itemBlue, 3, 0, 0, "------", 3, -3, 0, "Deal 3 damage to your opponent and gain 3 health. " },
{ 157, "Life Sap Drop", itemBlue, 3, 0, -1, "------", 1, 0, 1, "Deal 1 damage, gain 1 health, and draw a card. " },
{ 158, "Tome of Thunder", itemBlue, 3, 0, -4, "------", 0, 0, 0, "Deal 4 damage. " },
{ 159, "Vial of Soul Drain", itemBlue, 4, 0, -3, "------", 3, 0, 0, "Deal 3 damage and gain 3 health. " },
{ 160, "Minor Life Steal Potion", itemBlue, 2, 0, 0, "------", 2, -2, 0, "Deal 2 damage to your opponent and gain 2 health. " },
};

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

Card CardInfo::to_card() const
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
