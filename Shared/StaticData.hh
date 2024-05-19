#pragma once

#include <stdint.h>

const uint32_t ARENA_WIDTH = 3000;
const uint32_t ARENA_HEIGHT = 3000;

const float SERVER_DT = 0.05;//(ms)
const float PLAYER_ACCELERATION = 70 * SERVER_DT;//(1/s^2)
const float DEFAULT_FRICTION = 0.25;
const uint32_t MAX_SLOT_COUNT = 8;

#define REAL_TIME(st) (SERVER_DT * (st))
#define SERVER_TIME(rt) ((rt) / SERVER_DT)

class PetalId {
public:
    enum : uint8_t {
        kNone,
        kBasic,
        kFast,
        kHeavy,
        kTwin,
        kStinger,
        kLeaf,
        kIris,
        kWing,
        kMissile,
        kBubble,
        kPeas,
        kRock,
        kGrapes,
        kEgg,
        kTriplet,
        kHeaviest,
        kTringer,
        kBeetleEgg,
        kNumPetals
    };
};

class MobId {
public:
    enum : uint8_t {
        kBabyAnt,
        kWorkerAnt,
        kSoldierAnt,
        kBee,
        kLadybug,
        kBeetle,
        kHornet,
        kCentipede,
        kEvilCentipede,
        kSpider,
        kRock,
        kBoulder,
        kNumMobs
    };
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
    float radius;
    float reload; //in seconds
    float secondary_reload; //stuff like eggs + missile
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

extern struct PetalData PETAL_DATA[PetalId::kNumPetals];
extern struct MobData MOB_DATA[MobId::kNumMobs];

extern uint32_t RARITY_COLORS[kRarityId::kNumRarities];
extern float RARITY_SACRIFICE_XP[kRarityId::kNumRarities];

extern float xp_to_next_level(uint32_t);
extern uint32_t get_level_from_xp(float);