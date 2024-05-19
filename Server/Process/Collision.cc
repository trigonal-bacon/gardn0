#include <Server/Process/Process.hh>

#include <Server/EntityFunctions.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

#include <cmath>
#include <iostream>

static bool should_interact(Entity &ent1, Entity &ent2) {
    if (ent1.pending_delete || ent2.pending_delete) return false;
    if (ent1.team != ent2.team) return true;
    if (ent1.has_component(kPetal) || ent2.has_component(kPetal)) return false;
    return true;
}

static bool should_collide(Entity &ent1, Entity &ent2) {
    if (ent1.has_component(kDrop) || ent2.has_component(kDrop)) return false;
    return true;
}

static void pickup_drop(Simulation *simulation, Entity &player, Entity &drop) {
    if (!simulation->ent_alive(player.parent)) return;
    if (drop.despawn_ticks < SERVER_TIME(0.5)) return;
    Entity &camera = simulation->get_ent(player.parent);

    for (uint32_t i = 0; i < camera.loadout_count; ++i) {
        if (camera.loadout[i].id != PetalId::kNone) continue;
        camera.loadout[i].reset();
        camera.loadout[i].id = drop.drop_id;
        drop.set_x(player.x);
        drop.set_y(player.y);
        simulation->request_delete(drop.id);
        return;
    }
    for (uint32_t i = camera.loadout_count; i < camera.loadout_count + MAX_SLOT_COUNT; ++i) {
        if (camera.loadout_ids[i] != PetalId::kNone) continue;
        camera.set_loadout_ids(i, drop.drop_id);
        drop.set_x(player.x);
        drop.set_y(player.y);
        simulation->request_delete(drop.id);
        return;
    }
}

void on_collide(Simulation *simulation, Entity &ent1, Entity &ent2) {
    //check if collide (distance independent)
    if (!should_interact(ent1, ent2)) return;
    //distance dependent check
    Vector separation(ent1.x - ent2.x, ent1.y - ent2.y);
    float dist = ent1.radius + ent2.radius - separation.magnitude();
    if (dist < 0) return;
    if (should_collide(ent1, ent2)) { 
        if (separation.x == 0 && separation.y == 0) separation.unit_normal(frand() * 2 * 3.14159);
        separation.normalize();
        float ratio = ent2.mass / (ent1.mass + ent2.mass);
        ent1.collision_velocity.set(separation.x * (ratio * (dist)), separation.y * (ratio * (dist)));
        ent2.collision_velocity.set(separation.x * ((ratio - 1) * (dist)), separation.y * ((ratio - 1) * (dist)));
    }
    if (ent1.has_component(kHealth) && ent2.has_component(kHealth) && !(ent1.team == ent2.team)) {
        inflict_damage(simulation, ent1, ent2.id, ent2.damage);
        inflict_damage(simulation, ent2, ent1.id, ent1.damage);
    }
    if (ent1.has_component(kDrop) && ent2.has_component(kFlower)) {
        pickup_drop(simulation, ent2, ent1);
    }
    if (ent2.has_component(kDrop) && ent1.has_component(kFlower)) {
        pickup_drop(simulation, ent1, ent2);
    }
}