#pragma once

#include <Shared/EntityDef.hh>

class Simulation;
class Entity;

void inflict_damage(Simulation *, Entity &, EntityId &, float);
void inflict_heal(Simulation *, Entity &, float);

EntityId find_nearest_enemy(Simulation *, Entity &, float);