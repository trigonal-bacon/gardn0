#include <Shared/StaticData.hh>

#include <cmath>

struct PetalData PETAL_DATA[PetalId::kNumPetals] = {
    {"",        kCommon,    0,  0,  0,  0,   0, {}},
    {"Basic",   kCommon,    10, 10, 10, 2.5, 1, {}},
    {"Fast",    kCommon,    5,  7,  7,  1.0, 1, {}},
    {"Heavy",   kCommon,    20, 20, 12, 6.0, 1, {}},
    {"Rose",    kUnusual,   5,  5,  10, 3.5, 1, {.secondary_reload = 1.0, .heal = 10}},
    {"Twin",    kUnusual,   5,  7,  7,  1.0, 2, {}},
    {"Stinger", kUnusual,   5,  30, 7,  3.5, 1, {}},
    {"Leaf",    kUnusual,   8,  10, 10, 1.2, 1, {.heal = 1}},
    {"Iris",    kUnusual,   5,  5,  7,  5.0, 1, {}},
    {"Wing",    kRare,      15, 15, 15, 1.5, 1, {}},
    {"Missile", kRare,      35, 15, 15, 2.5, 1, {.secondary_reload = 0.5}},
    {"Bubble",  kRare,      1,  1,  12, 3.5, 1, {.secondary_reload = 0.5}},
    {"Azalea",  kRare,      5,  5,  7,  3.5, 3, {.secondary_reload = 1.0, .clump_radius = 10, .heal = 4}},
    {"Peas",    kRare,      5,  5,  7,  1.5, 4, {.secondary_reload = 0.5, .clump_radius = 8}},
    {"Rock",    kRare,      100,15, 12, 10., 1, {}},
    {"Cactus",  kRare,      30, 2,  15, 1.0, 1, {}},
    {"Grapes",  kEpic,      2,  5,  7,  1.5, 4, {.secondary_reload = 0.5, .clump_radius = 8}},
    {"Egg",     kEpic,      50, 1,  14, 4.0, 1, {.secondary_reload = 10}},
    {"Triplet", kEpic,      50, 1,  10, 1.0, 3, {}},
    {"Heaviest",kEpic,      250,10, 16, 12., 1, {}},
    {"Azalea",  kEpic,      5, 5,   10, 3.5, 1, {.secondary_reload = 1.0, .heal = 20}},
    {"Stinger", kLegendary, 5,  30, 7,  3.5, 3, {.clump_radius = 10}},
    {"Cactus",  kLegendary, 30, 2,  15, 1.0, 3, {.clump_radius = 10}},
    {"Egg",     kLegendary, 50, 1,  15, 4.0, 1, {.secondary_reload = 1.0}},
};

struct MobData MOB_DATA[MobId::kNumMobs] = {
    {"Baby Ant", kCommon, 10, 10, 14, 1, {{PetalId::kFast, 0.36},{PetalId::kTwin, 0.12},{PetalId::kLeaf, 0.09}}},
    {"Worker Ant", kCommon, 15, 10, 14, 2, {{PetalId::kFast, 0.48},{PetalId::kTwin, 0.16},{PetalId::kLeaf, 0.12}}},
    {"Soldier Ant", kUnusual, 30, 10, 14, 6, {{PetalId::kTwin, 0.16},{PetalId::kWing, 0.008},{PetalId::kTriplet,0.0004}}},
    {"Bee", kCommon, 15, 40, 25, 3, {{PetalId::kFast, 0.36},{PetalId::kStinger, 0.12},{PetalId::kBubble,0.006},{PetalId::kTringer, 0.0002}}},
    {"Ladybug", kCommon, 25, 10, 30, 2, {{PetalId::kFast, 0.36},{PetalId::kTwin, 0.04},{PetalId::kRose, 0.12},{PetalId::kWing, 0.006}}},
    {"Beetle", kRare, 40, 30, 35, 9, {{PetalId::kIris, 0.48},{PetalId::kWing,0.024},{PetalId::kTriplet, 0.001}}},
    {"Hornet", kRare, 40, 30, 25, 15, {{PetalId::kMissile, 0.18},{PetalId::kBubble, 0.12}}},
    {"Centipede", kCommon, 25, 10, 35, 3, {{PetalId::kFast, 0.24},{PetalId::kLeaf, 0.08},{PetalId::kPeas, 0.006}}},
    {"Evil Centipede", kUnusual, 25, 10, 35, 3, {{PetalId::kIris, 0.48},{PetalId::kGrapes, 0.005}}},
    {"Spider", kUnusual, 25, 20, 15, 7, {{PetalId::kIris, 0.48},{PetalId::kStinger, 0.24}}},
    {"Rock", kCommon, 25, 10, 25, 2, {{PetalId::kFast, 0.24},{PetalId::kHeavy, 0.36},{PetalId::kRock, 0.005}}},
    {"Boulder", kUnusual, 60, 10, 45, 5, {{PetalId::kHeavy, 1.00},{PetalId::kRock, 0.04},{PetalId::kHeaviest, 0.0006}}},
    {"Cactus", kCommon, 40, 30, 50, 4, {{PetalId::kStinger, 0.12},{PetalId::kCactus, 0.04},{PetalId::kTricac, 0.00005}}},
    {"Ladybug", kUnusual, 40, 10, 30, 4, {{PetalId::kAzalea, 0.12},{PetalId::kWing, 0.06},{PetalId::kEAzalea, 0.002}}},
    {"Beetle", kEpic, 300, 30, 75, 50, {{PetalId::kIris, 1.00},{PetalId::kWing, 0.82},{PetalId::kTriplet, 0.02},{PetalId::kBeetleEgg, 0.005}}},
    {"Ladybug", kLegendary, 500, 10, 100, 100, {{PetalId::kRose, 1.00},{PetalId::kWing, 1.00},{PetalId::kAzalea, 1.00},{PetalId::kEAzalea, 1.00}}},
};

uint32_t RARITY_COLORS[kRarityId::kNumRarities] = { 0xff7eef6d, 0xffffe65d, 0xff4d52e3, 0xff861fde, 0xffde1f1f, 0xff1fdbde };//, 0xffff2b75, 0xfff70fb6};

float RARITY_SACRIFICE_XP[kRarityId::kNumRarities] = {
    1,
    5,
    40,
    500,
    7500,
    100000
};

float xp_to_next_level(uint32_t curr_level) {
    return 1 + 3 * curr_level * powf(1.06, curr_level);
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