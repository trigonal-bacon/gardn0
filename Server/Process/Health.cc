#include <Server/Process/Process.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

#include <cmath>
#include <iostream>

void tick_health_behavior(Simulation *simulation, Entity &ent) {
    if (ent.applied_poison.ticks_left == 0) ent.applied_poison.reset();
    else {
        float h = fclamp(ent.health - ent.applied_poison.damage, 0, ent.max_health);
        ent.set_health(h);
        --ent.applied_poison.ticks_left;
        if (ent.applied_poison.ticks_left % ((uint32_t) SERVER_TIME(1)) == 0) ent.set_damaged(1);
        if (h == 0) simulation->request_delete(ent.id);
    }
    if (ent.immunity_ticks > 0) --ent.immunity_ticks;
}