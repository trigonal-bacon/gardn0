#include <Server/Process/Process.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

#include <cmath>
#include <iostream>

void tick_petal(Simulation *simulation, Entity &petal, uint32_t rot_pos, uint32_t secondary_pos) {
    assert(simulation->ent_alive(petal.id));
    assert(simulation->ent_alive(petal.parent));
    Entity &player = simulation->get_ent(petal.parent);
    Vector move_to;
    float mag = (player.input & 1) ? 150 : (player.input & 2) ? 50 : 75;
    move_to.unit_normal(M_PI * 2 * rot_pos / player.rotation_count + player.rotation_angle).set_magnitude(mag);
    move_to.x += player.x - petal.x;
    move_to.y += player.y - petal.y;
    if (PETAL_DATA[petal.petal_id].clump_radius != 0) {
        Vector clump_vector;
        clump_vector.unit_normal(player.rotation_angle * 0.75 + 2 * M_PI * secondary_pos / PETAL_DATA[petal.petal_id].count).set_magnitude(PETAL_DATA[petal.petal_id].clump_radius);
        move_to += clump_vector;
    }
    move_to *= 0.6;
    petal.acceleration = move_to;
    petal.set_angle(fmod(petal.angle + 2 * SERVER_DT, M_PI * 2));
}

static void petal_behavior(Simulation *simulation, Entity &player) {
    //player guaranteed to be ALIVE
    assert(simulation->ent_exists(player.parent));
    Entity &camera = simulation->get_ent(player.parent);
    uint32_t rotation_pos = 0;
    for (uint32_t i = 0; i < camera.loadout_count; ++i) {
        LoadoutSlot &slot = camera.loadout[i];
        if (slot.id == 0) continue;
        for (uint32_t j = 0; j < PETAL_DATA[slot.id].count; ++j) {
            LoadoutPetal &petal_slot = slot.petals[j];
            if (!simulation->ent_alive(petal_slot.ent_id)) {
                if (++petal_slot.reload >= PETAL_DATA[slot.id].reload / SERVER_DT) {
                    Entity &petal = simulation->alloc_ent();
                    petal.add_component(kPhysics);
                    petal.set_x(player.x);
                    petal.set_y(player.y);
                    petal.set_radius(PETAL_DATA[slot.id].radius);
                    petal.friction = 0.25;
                    petal.add_component(kRelations);
                    petal.set_parent(player.id);
                    petal.set_team(player.parent);
                    petal.add_component(kPetal);
                    petal.set_petal_id(slot.id);
                    petal.add_component(kHealth);
                    petal.set_max_health(PETAL_DATA[slot.id].health);
                    petal.set_health(PETAL_DATA[slot.id].health);
                    petal.damage = PETAL_DATA[slot.id].damage;
                    std::cout << "petal create " << petal.id.id << '\n';
                    petal_slot.ent_id = petal.id;
                }
            } else {
                Entity &petal = simulation->get_ent(petal_slot.ent_id);
                petal_slot.reload = 0;
                tick_petal(simulation, petal, rotation_pos, j);
            }
            if (PETAL_DATA[slot.id].clump_radius == 0) ++rotation_pos;
        }
        if (PETAL_DATA[slot.id].clump_radius != 0) ++rotation_pos;
    }
    player.rotation_count = rotation_pos;
    player.rotation_angle += 2.5 * SERVER_DT;
    //player.rotation_angle = fmod(player.rotation_angle, 2 * M_PI);
}

void tick_petal_behavior(Simulation *sim) {
    for (uint32_t i = 0; i < sim->active_entities.length; ++i) {
        Entity &ent = sim->get_ent(sim->active_entities[i]);
        if (ent.has_component(kFlower) && !ent.pending_delete) petal_behavior(sim, ent);
        else if (ent.has_component(kPetal) && !sim->ent_alive(ent.parent)) sim->request_delete(ent.id);
    }
}