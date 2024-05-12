#pragma once

#include <stdint.h>

static const uint32_t ARENA_WIDTH = 1024;
static const uint32_t ARENA_HEIGHT = 1024;

static const float SERVER_DT = 0.05;//(ms)
static const float PLAYER_ACCELERATION = 100;//(1/s^2)

enum kPetalId : uint8_t {
    kNone,
    kBasic,
    kFast,
    kTwin,
    kStinger,
    kTringer,
    kNumPetals
};

enum kMobId : uint8_t {
    kBabyAnt,
    kNumMobs
};

enum kRarityId : uint8_t {
    kCommon,
    kUnusual,
    kRare,
    kNumRarities
};

struct PetalData {
    uint8_t id;
    uint8_t rarity;
    float health;
    float damage;
    float reload; //in seconds
    float radius;
    uint8_t count;
    float clump_radius;
};

struct MobDrop {
    uint8_t id;
    float chance;
};

struct MobData {
    uint8_t id;
    uint8_t rarity;
    float health;
    float damage;
    float radius;
    MobDrop drops[4];
};

extern struct PetalData PETAL_DATA[kPetalId::kNumPetals];
extern struct MobData MOB_DATA[kMobId::kNumMobs];