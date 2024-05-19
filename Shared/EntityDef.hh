#pragma once

#include <stdint.h>

enum kMobAiState {
    kIdle,
    kIdleMoving,
    kIdleMoving2,
    kAggro,
    kReturning,
    kFiring
};

bool ai_state_is_passive(uint8_t);

class EntityId {
public:
    uint16_t id = 0;
    uint16_t hash = 0;
    EntityId();
    EntityId(uint16_t, uint16_t);
    bool null() const;
    void operator=(const EntityId &);
    void operator=(uint32_t);
};

extern EntityId NULL_ENTITY;

class PoisonDefinition {
public:
    float damage;
    uint32_t ticks;
    PoisonDefinition();
    void define(float, uint32_t);
    bool has();
};

class AppliedPoison {
public:
    EntityId dealer;
    float damage;
    uint32_t ticks_left;
    AppliedPoison();
    void reset();
};

bool operator<(const EntityId &, const EntityId &);
bool operator==(const EntityId &, const EntityId &);

class LoadoutPetal {
public:
    uint32_t reload;
    EntityId ent_id;
};

class LoadoutSlot {
public:
    uint8_t id;
    LoadoutPetal petals[5];
    LoadoutSlot();
    void reset();
};