#include <Shared/StaticData.hh>

struct PetalData PETAL_DATA[kPetalId::kNumPetals] = {
    {kNone, kCommon, 0, 0, 0, 0, 0, 0},
    {kBasic, kCommon, 10, 10, 2, 10, 1, 0},
    {kFast, kCommon, 5, 7, 1, 7, 1, 0},
    {kTwin, kUnusual, 5, 7, 1, 7, 2, 0},
    {kStinger, kUnusual, 5, 30, 3.5, 7, 1, 0},
    {kTringer, kUnusual, 5, 30, 3.5, 7, 3, 10}
};

struct MobData MOB_DATA[kMobId::kNumMobs] = {
    {kBabyAnt, kCommon, 10, 10, 14, {{kFast, 1},{kTwin, 0.05}}}
};
