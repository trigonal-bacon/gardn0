#pragma once

#include <stdint.h>

static const uint32_t ARENA_WIDTH = 1500;
static const uint32_t ARENA_HEIGHT = 1500;

static const float SERVER_DT = 0.05;//(ms)
static const float PLAYER_ACCELERATION = 30 * SERVER_DT;//(1/s^2)
static const float DEFAULT_FRICTION = 0.2;

#define REAL_TIME(st) (SERVER_DT * (st))
#define SERVER_TIME(rt) ((rt) / SERVER_DT)

enum kPetalId : uint8_t {
    kNone,
    kBasic,
    kFast,
    kTwin,
    kStinger,
    kLeaf,
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
    kEpic,
    kLegendary,
    kMythic,
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
    float xp;
    MobDrop drops[4];
};

extern struct PetalData PETAL_DATA[kPetalId::kNumPetals];
extern struct MobData MOB_DATA[kMobId::kNumMobs];

extern uint32_t RARITY_COLORS[kRarityId::kNumRarities];