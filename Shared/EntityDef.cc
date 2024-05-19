#include <Shared/EntityDef.hh>

#include <Shared/StaticData.hh>

EntityId NULL_ENTITY(0, 0);

EntityId::EntityId() {
}

EntityId::EntityId(uint16_t id, uint16_t hash) : id(id), hash(hash) {
}

bool EntityId::null() const {
    return id == 0;
}

void EntityId::operator=(const EntityId &o) {
    id = o.id;
    hash = o.hash;
}


void EntityId::operator=(uint32_t v) {
    id = hash = 0;
}

bool operator<(const EntityId &a, const EntityId &b) {
    return (a.hash * 65536 + a.id) < (b.hash * 65536 + b.id);
}

bool operator==(const EntityId &a, const EntityId &b) {
    return a.id == b.id && a.hash == b.hash;
}

LoadoutSlot::LoadoutSlot() {
    reset();
}

void LoadoutSlot::reset() {
    id = PetalId::kNone;
    for (uint32_t i = 0; i < 3; ++i) {
        petals[i].reload = 0;
        petals[i].ent_id = NULL_ENTITY;
    }
}

PoisonDefinition::PoisonDefinition() : damage(0), ticks(0) {}

void PoisonDefinition::define(float d, uint32_t t) {
    damage = d;
    ticks = t;
}

bool PoisonDefinition::has() {
    return damage != 0;
}

AppliedPoison::AppliedPoison() : dealer(NULL_ENTITY), damage(0), ticks_left(0) {}

void AppliedPoison::reset() { 
    damage = 0;
    ticks_left = 0;
}

bool ai_state_is_passive(uint8_t state) {
    if (state == kIdle || state == kIdleMoving) return true;
    return false;
}