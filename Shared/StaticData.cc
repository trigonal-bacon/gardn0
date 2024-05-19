#include <Shared/StaticData.hh>

struct PetalData PETAL_DATA[kPetalId::kNumPetals] = {
    {"",        kCommon,    0,  0,  0,   0,   0.0, 0, 0},
    {"Basic",   kCommon,    10, 10, 10, 2.5,  0.0, 1, 0},
    {"Fast",    kCommon,    5,  7,  7,  1.0,  0.0, 1, 0},
    {"Twin",    kUnusual,   5,  7,  7,  1.0,  0.0, 2, 0},
    {"Stinger", kUnusual,   5,  30, 7,  3.5,  0.0, 1, 0},
    {"Leaf",    kUnusual,   8,  10, 10, 1.2,  0.0, 1, 0},
    {"Iris",    kUnusual,   5,  5,  7,  5.0,  0.0, 1, 0},
    {"Wing",    kRare,      15, 15, 15, 1.5,  0.0, 1, 0},
    {"Missile", kRare,      35, 15, 15, 2.5,  0.5, 1, 0},
    {"Bubble",  kRare,      1,  1,  12, 3.5,  0.5, 1, 0},
    {"Egg",     kEpic,      50, 1,  14, 4.0,  1.0, 1, 0},
    {"Triplet", kEpic,      50, 1,  10, 1.0,  0.0, 3, 0},
    {"Stinger", kLegendary, 5,  30, 7,  3.5,  0.0, 3, 10},
    {"Egg",     kLegendary, 50, 1,  15, 4.0,  1.0, 1, 0},
};

struct MobData MOB_DATA[kMobId::kNumMobs] = {
    {"Baby Ant", kCommon, 10, 10, 14, 1, {{kEgg, 1.00},{kTwin, 0.12},{kLeaf, 0.08}}},
    {"Worker Ant", kCommon, 15, 10, 14, 2, {{kFast, 0.44},{kTwin, 0.16},{kLeaf, 0.12}}},
    {"Soldier Ant", kUnusual, 40, 10, 14, 6, {{kFast, 0.44},{kTwin, 0.12},{kWing, 0.008},{kTriplet,0.0006}}},
    {"Bee", kCommon, 15, 40, 25, 3, {{kFast, 0.36},{kStinger, 0.12},{kTringer, 0.0001}}},
    {"Ladybug", kCommon, 25, 10, 30, 2, {{kFast, 0.36},{kStinger, 0.12},{kBubble, 1.00}}},
    {"Beetle", kRare, 40, 30, 35, 9, {{kBeetleEgg, 1.36},{kIris, 1.12},{kMissile, 1.00}}}
};

uint32_t RARITY_COLORS[kRarityId::kNumRarities] = { 0xff7eef6d, 0xffffe65d, 0xff4d52e3, 0xff861fde, 0xffde1f1f, 0xff1fdbde };//, 0xffff2b75, 0xfff70fb6};

float RARITY_SACRIFICE_XP[kRarityId::kNumRarities] = {
    1,
    5,
    50,
    750,
    5000,
    50000
};

float xp_to_next_level(uint32_t curr_level) {
    return 5 + 2 * curr_level * curr_level;
}

uint32_t get_level_from_xp(float xp) {
    uint32_t level = 1;
    xp -= xp_to_next_level(level);
    while (xp > 0) {
        ++level;
        xp -= xp_to_next_level(level);
    }
    return level;
}