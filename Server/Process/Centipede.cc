#include <Server/Process/Process.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

#include <iostream>

void tick_centipede_behavior(Simulation *sim, Entity &ent) {
    if (!sim->ent_alive(ent.head_node)) return;
    Entity &par = sim->get_ent(ent.head_node);
    Vector diff(ent.x - par.x, ent.y - par.y);
    diff.set_magnitude(ent.radius + par.radius + 1);
    ent.set_x(par.x + diff.x);
    ent.set_y(par.y + diff.y);
    ent.set_angle(diff.angle() + M_PI);
}