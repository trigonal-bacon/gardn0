#pragma once

class Simulation;
class Entity;

void tick_entity_motion(Simulation *, Entity &);
void on_collide(Simulation *, Entity &, Entity &);