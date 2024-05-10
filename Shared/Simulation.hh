#pragma once

#include <Shared/Entity.hh>
#include <Shared/Helpers.hh>

static const int ENTITY_CAP = 1024;
class Simulation {
public:
    uint8_t entity_tracker[ENTITY_CAP] = {0};
    uint32_t hash_tracker[ENTITY_CAP] = {0};
    Entity entities[ENTITY_CAP];
    StaticArray<EntityId, ENTITY_CAP> active_entities;
    StaticArray<EntityId, ENTITY_CAP> pending_delete;
    Simulation();
    Entity &alloc_ent();
    Entity &get_ent(EntityId);
    void force_alloc_ent(EntityId);
    uint8_t ent_exists(EntityId);
    uint8_t ent_alive(EntityId);
    void request_delete(EntityId);
    void delete_ent(EntityId);
    void pre_tick();
    void tick();
#ifdef SERVER_SIDE
    void post_tick();
#endif
};