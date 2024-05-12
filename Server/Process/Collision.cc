#include <Server/Process/Process.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

static bool should_collide(Entity &ent1, Entity &ent2) {
    if (ent1.pending_delete || ent2.pending_delete) return false;
    return true;
}

void on_collide(Simulation *simulation, Entity &ent1, Entity &ent2) {
    //check if collide (distance independent)
    if (!should_collide(ent1, ent2)) return;
    //distance dependent check
    Vector separation(ent1.x - ent2.x, ent1.y - ent2.y);
    float dist = ent1.radius + ent2.radius - separation.magnitude();
    if (dist < 0) return;
    separation.normalize();
    float ratio = ent2.mass / (ent1.mass + ent2.mass);
    ent1.collision_velocity.set(separation.x * ratio * dist, separation.y * ratio * dist);
    ent2.collision_velocity.set(separation.x * (ratio - 1) * dist, separation.y * (ratio - 1) * dist);
}