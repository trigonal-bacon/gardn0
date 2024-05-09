#pragma once

#include <Shared/Entity.hh>

static const int ENTITY_CAP = 1024;
class Simulation {
public:
    uint8_t entity_tracker[ENTITY_CAP] = {0};
    uint8_t hash_tracker[ENTITY_CAP] = {0};
    Entity entities[ENTITY_CAP];
    Simulation();
    uint32_t alloc_ent();
    uint8_t ent_alive(uint32_t);
    void delete_ent(uint32_t);
};