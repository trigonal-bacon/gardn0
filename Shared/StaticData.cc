#include <Shared/StaticData.hh>

struct PetalData PETAL_DATA[kPetalId::kNumPetals] = {
    {kNone, kCommon, 0, 0, 0, 0, 0, 0},
    {kBasic, kCommon, 10, 10, 2, 10, 1, 0},
    {kFast, kCommon, 5, 7, 1, 7, 1, 0},
    {kTwin, kUnusual, 5, 7, 1, 7, 2, 0},
    {kStinger, kUnusual, 5, 30, 3.5, 7, 1, 0},
    {kLeaf, kUnusual, 8, 10, 1.5, 10, 1, 0},
    {kTringer, kLegendary, 5, 30, 3.5, 7, 3, 10}
};

struct MobData MOB_DATA[kMobId::kNumMobs] = {
    {kBabyAnt, kCommon, 10, 10, 14, 1, {{kFast, 1},{kTwin, 0.5},{kLeaf, 0.16}}}
};

uint32_t RARITY_COLORS[kRarityId::kNumRarities] = {
    0xff7eef6d, 0xffffe65d, 0xff4d52e3, 0xff861fde,
   0xffde1f1f, 0xff1fdbde};//, 0xffff2b75, 0xfff70fb6};