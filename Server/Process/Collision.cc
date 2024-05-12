#include <Server/Process/Process.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

#include <cmath>
#include <iostream>

static bool should_collide(Entity &ent1, Entity &ent2) {
    if (ent1.pending_delete || ent2.pending_delete) return false;
    if (ent1.has_component(kDrop) || ent2.has_component(kDrop)) return false;
    if (ent1.team == ent2.team) return false;
    return true;
}

static void damage(Simulation *simulation, Entity &reciever, EntityId &dealer, float damage) {
    assert(reciever.has_component(kHealth));
    if (reciever.pending_delete) return;
    float rh = fclamp(reciever.health - damage, 0, reciever.max_health);
    reciever.set_health(rh);
    if (rh == 0) simulation->request_delete(reciever.id);
}

void on_collide(Simulation *simulation, Entity &ent1, Entity &ent2) {
    //check if collide (distance independent)
    if (!should_collide(ent1, ent2)) return;
    //distance dependent check
    Vector separation(ent1.x - ent2.x, ent1.y - ent2.y);
    float dist = ent1.radius + ent2.radius - separation.magnitude();
    if (dist < 0) return;
    if (separation.x == 0 && separation.y == 0) separation.unit_normal(frand() * 2 * 3.14159);
    separation.normalize();
    float ratio = ent2.mass / (ent1.mass + ent2.mass);
    ent1.collision_velocity.set(separation.x * ratio * dist, separation.y * ratio * dist);
    ent2.collision_velocity.set(separation.x * (ratio - 1) * dist, separation.y * (ratio - 1) * dist);

    if (ent1.has_component(kHealth) && ent2.has_component(kHealth) && !(ent1.team == ent2.team)) {
        if (ent1.pending_delete || ent2.pending_delete) return;
        damage(simulation, ent1, ent2.id, ent2.damage);
        damage(simulation, ent2, ent1.id, ent1.damage);
    }
}