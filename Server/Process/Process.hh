#pragma once

class Simulation;
class Entity;

void tick_entity_motion(Simulation *, Entity &);
void on_collide(Simulation *, Entity &, Entity &);

void tick_petal_behavior(Simulation *);
void tick_drop_behavior(Simulation *, Entity &);
void tick_health_behavior(Simulation *, Entity &);

void tick_mob_ai(Simulation *, Entity &);