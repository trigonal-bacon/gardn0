#include <Shared/StaticData.hh>

struct PetalData PETAL_DATA[PetalId::kNumPetals] = {
    {"",        kCommon,    0,  0,  0,   0,   0.0, 0, 0},
    {"Basic",   kCommon,    10, 10, 10, 2.5,  0.0, 1, 0},
    {"Fast",    kCommon,    5,  7,  7,  1.0,  0.0, 1, 0},
    {"Heavy",   kCommon,    20, 20, 12, 6.0,  0.0, 1, 0},
    {"Twin",    kUnusual,   5,  7,  7,  1.0,  0.0, 2, 0},
    {"Stinger", kUnusual,   5,  30, 7,  3.5,  0.0, 1, 0},
    {"Leaf",    kUnusual,   8,  10, 10, 1.2,  0.0, 1, 0},
    {"Iris",    kUnusual,   5,  5,  7,  5.0,  0.0, 1, 0},
    {"Wing",    kRare,      15, 15, 15, 1.5,  0.0, 1, 0},
    {"Missile", kRare,      35, 15, 15, 2.5,  0.5, 1, 0},
    {"Bubble",  kRare,      1,  1,  12, 3.5,  0.5, 1, 0},
    {"Peas",    kRare,      5,  5,  7,  1.5,  0.0, 4, 8},
    {"Rock",    kRare,      100,15, 12, 10.,  0.0, 1, 0},
    {"Grapes",  kEpic,      2,  5,  7,  1.5,  0.0, 4, 8},
    {"Egg",     kEpic,      50, 1,  14, 4.0,  1.0, 1, 0},
    {"Triplet", kEpic,      50, 1,  10, 1.0,  0.0, 3, 0},
    {"Heaviest",kEpic,      250,10, 16, 12.,  0.0, 1, 0},
    {"Stinger", kLegendary, 5,  30, 7,  3.5,  0.0, 3, 10},
    {"Egg",     kLegendary, 50, 1,  15, 4.0,  1.0, 1, 0},
};

struct MobData MOB_DATA[MobId::kNumMobs] = {
    {"Baby Ant", kCommon, 10, 10, 14, 1, {{PetalId::kFast, 0.36},{PetalId::kTwin, 0.12},{PetalId::kLeaf, 0.09}}},
    {"Worker Ant", kCommon, 15, 10, 14, 2, {{PetalId::kFast, 0.48},{PetalId::kTwin, 0.16},{PetalId::kLeaf, 0.12}}},
    {"Soldier Ant", kUnusual, 30, 10, 14, 6, {{PetalId::kTwin, 0.16},{PetalId::kWing, 0.008},{PetalId::kTriplet,0.0004}}},
    {"Bee", kCommon, 15, 40, 25, 3, {{PetalId::kFast, 0.36},{PetalId::kStinger, 0.12},{PetalId::kTringer, 0.0002}}},
    {"Ladybug", kCommon, 25, 10, 30, 2, {{PetalId::kFast, 0.36},{PetalId::kStinger, 0.12},{PetalId::kBubble, 1.00}}},
    {"Beetle", kRare, 40, 30, 35, 9, {{PetalId::kIris, 0.48},{PetalId::kTriplet, 0.024}}},
    {"Hornet", kRare, 40, 30, 25, 15, {{PetalId::kMissile, 0.18},{PetalId::kBubble, 0.12}}},
    {"Centipede", kCommon, 25, 10, 35, 3, {{PetalId::kFast, 0.24},{PetalId::kLeaf, 0.08},{PetalId::kPeas, 0.006}}},
    {"Evil Centipede", kUnusual, 25, 10, 35, 3, {{PetalId::kIris, 0.48},{PetalId::kGrapes, 0.005}}},
    {"Spider", kUnusual, 25, 20, 15, 7, {{PetalId::kIris, 0.48},{PetalId::kStinger, 0.24}}},
    {"Rock", kCommon, 25, 10, 25, 2, {{PetalId::kFast, 0.24},{PetalId::kHeavy, 0.36},{PetalId::kRock, 0.005}}},
    {"Boulder", kUnusual, 60, 10, 45, 5, {{PetalId::kHeavy, 1.00},{PetalId::kRock, 0.04},{PetalId::kHeaviest, 0.0006}}}
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