#include <Server/Process/Process.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

#include <cmath>
#include <iostream>

void tick_drop_behavior(Simulation *simulation, Entity &ent) {
    if (ent.despawn_ticks == 0) {
        ent.set_angle(frand() * 0.2 - 0.1);
        ent.set_radius(20);
    }
    if (++ent.despawn_ticks >= SERVER_TIME(10 + 10 * PETAL_DATA[ent.drop_id].rarity)) {
        simulation->request_delete(ent.id);
        return;
    }
    
}