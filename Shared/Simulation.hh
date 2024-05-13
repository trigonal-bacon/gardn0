#pragma once

#include <Shared/Entity.hh>
#include <Shared/Helpers.hh>
#include <Shared/StaticData.hh>

#ifdef SERVER_SIDE
#include <Server/SpatialHash.hh>

class Client;
#endif

static const uint32_t ENTITY_CAP = 1024;

class Simulation {
public:
SERVER_ONLY(SpatialHash spatial_hash;)
    uint8_t entity_tracker[ENTITY_CAP] = {0};
    uint32_t hash_tracker[ENTITY_CAP] = {0};
    Entity entities[ENTITY_CAP];
    StaticArray<EntityId, ENTITY_CAP> active_entities;
    StaticArray<EntityId, ENTITY_CAP> pending_delete;
    Simulation();
    Entity &alloc_ent();
    Entity &get_ent(EntityId &);
    void force_alloc_ent(EntityId &);
    uint8_t ent_exists(EntityId &);
    uint8_t ent_alive(EntityId &);
    void request_delete(EntityId &);
    void delete_ent(EntityId &); //DANGEROUS
    void pre_tick();
    void tick();
    void post_tick();
#ifdef SERVER_SIDE
    void update_client(Client *);
    Entity &alloc_mob(uint8_t);
    Entity &alloc_player(Entity &);
    Entity &alloc_petal(uint8_t);
#else
    void tick_lerp(double);
#endif
};