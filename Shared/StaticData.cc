#include <Shared/StaticData.hh>

struct PetalData PETAL_DATA[kPetalId::kNumPetals] = {
    {"", kCommon, 0, 0, 0, 0, 0, 0},
    {"Basic", kCommon, 10, 10, 2, 10, 1, 0},
    {"Fast", kCommon, 5, 7, 1, 7, 1, 0},
    {"Twin", kUnusual, 5, 7, 1, 7, 2, 0},
    {"Stinger", kUnusual, 5, 30, 3.5, 7, 1, 0},
    {"Leaf", kUnusual, 8, 10, 1.5, 10, 1, 0},
    {"Triplet", kEpic, 5, 7, 1, 7, 3, 0},
    {"Stinger", kLegendary, 5, 30, 3.5, 7, 3, 10}
};

struct MobData MOB_DATA[kMobId::kNumMobs] = {
    {"Baby Ant", kCommon, 10, 10, 14, 1, {{kFast, 0.36},{kTwin, 0.12},{kLeaf, 0.08},{kTriplet,0.0006}}},
    {"Worker Ant", kCommon, 15, 10, 14, 2, {{kFast, 0.44},{kTwin, 0.12},{kLeaf, 0.08},{kTriplet,0.0006}}},
    {"Bee", kCommon, 15, 40, 25, 3, {{kFast, 0.36},{kStinger, 0.12},{kTringer, 0.0001}}}
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