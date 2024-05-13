#pragma once

class Simulation;
class Entity;
class EntityId;

void inflict_damage(Simulation *, Entity &, EntityId &, float);
void inflict_heal(Simulation *, Entity &, float);
