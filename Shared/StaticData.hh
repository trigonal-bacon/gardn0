#pragma once

#include <stdint.h>

static const uint32_t ARENA_WIDTH = 1500;
static const uint32_t ARENA_HEIGHT = 1500;

static const float SERVER_DT = 0.05;//(ms)
static const float PLAYER_ACCELERATION = 60 * SERVER_DT;//(1/s^2)
static const float DEFAULT_FRICTION = 0.2;
static const uint32_t MAX_SLOT_COUNT = 8;

#define REAL_TIME(st) (SERVER_DT * (st))
#define SERVER_TIME(rt) ((rt) / SERVER_DT)

enum kPetalId : uint8_t {
    kNone,
    kBasic,
    kFast,
    kTwin,
    kStinger,
    kLeaf,
    kTriplet,
    kTringer,
    kNumPetals
};

enum kMobId : uint8_t {
    kBabyAnt,
    kWorkerAnt,
    kBee,
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
    char const *name;
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
    char const *name;
    uint8_t rarity;
    float health;
    float damage;
    float radius;
    float xp;
    MobDrop drops[6];
};

extern struct PetalData PETAL_DATA[kPetalId::kNumPetals];
extern struct MobData MOB_DATA[kMobId::kNumMobs];

extern uint32_t RARITY_COLORS[kRarityId::kNumRarities];
extern float RARITY_SACRIFICE_XP[kRarityId::kNumRarities];

extern float xp_to_next_level(uint32_t);
extern uint32_t get_level_from_xp(float);