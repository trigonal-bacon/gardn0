#pragma once

#include <stdint.h>

const uint32_t ARENA_WIDTH = 3000;
const uint32_t ARENA_HEIGHT = 3000;

const float SERVER_DT = 0.05;//(ms)
const float PLAYER_ACCELERATION = 3.95;//(1/s^2)
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
        kRose,
        kTwin,
        kStinger,
        kLeaf,
        kIris,
        kWing,
        kMissile,
        kBubble,
        kAzalea,
        kPeas,
        kRock,
        kCactus,
        kGrapes,
        kEgg,
        kTriplet,
        kHeaviest,
        kEAzalea,
        kPoisonCactus,
        kTringer,
        kTricac,
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
        kCactus,
        kDarkLadybug,
        kMassiveBeetle,
        kMassiveLadybug,
        kAntHole,
        kQueenAnt,
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

struct PetalDataExtras {
    float secondary_reload = 0;
    float clump_radius = 0;
    float poison_damage = 0;
    float poison_time = 0.01;
    float heal = 0;
};

struct PetalData {
    char const *name;
    uint8_t rarity;
    float health;
    float damage;
    float radius;
    float reload; //in seconds
    uint8_t count;
    struct PetalDataExtras extras;
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