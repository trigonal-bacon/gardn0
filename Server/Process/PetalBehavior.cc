#include <Server/Process/Process.hh>

#include <Server/EntityFunctions.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

#include <Shared/Helpers.hh>

#include <cmath>
#include <iostream>

void tick_petal(Simulation *simulation, Entity &petal, uint32_t rot_pos, uint32_t secondary_pos) {
    assert(simulation->ent_alive(petal.id));
    assert(simulation->ent_alive(petal.parent));
    Entity &player = simulation->get_ent(petal.parent);
    Vector move_to;
    float mag = (BIT_AT(player.input, 0)) ? 150 : (BIT_AT(player.input, 1)) ? 50 : 75;
    move_to.unit_normal(M_PI * 2 * rot_pos / player.rotation_count + player.rotation_angle).set_magnitude(mag);
    move_to.x += player.x - petal.x;
    move_to.y += player.y - petal.y;
    if (PETAL_DATA[petal.petal_id].clump_radius != 0) {
        Vector clump_vector;
        clump_vector.unit_normal(player.rotation_angle * 0.75 + 2 * M_PI * secondary_pos / PETAL_DATA[petal.petal_id].count).set_magnitude(PETAL_DATA[petal.petal_id].clump_radius);
        move_to += clump_vector;
    }
    move_to *= 0.5;
    petal.acceleration = move_to;
    petal.set_angle(fmod(petal.angle + REAL_TIME(2), M_PI * 2));
}

static void petal_behavior(Simulation *simulation, Entity &player) {
    //player guaranteed to be ALIVE
    assert(simulation->ent_exists(player.parent));
    Entity &camera = simulation->get_ent(player.parent);
    uint32_t rotation_pos = 0;
    for (uint32_t i = 0; i < camera.loadout_count; ++i) {
        LoadoutSlot &slot = camera.loadout[i];
        camera.set_loadout_ids(i, slot.id);
        if (slot.id == 0) continue;
        if (slot.id == kLeaf) inflict_heal(simulation, player, REAL_TIME(1));
        uint32_t max = SERVER_TIME(PETAL_DATA[slot.id].reload);
        uint32_t min = max;
        for (uint32_t j = 0; j < PETAL_DATA[slot.id].count; ++j) {
            LoadoutPetal &petal_slot = slot.petals[j];
            if (petal_slot.reload < min) min = petal_slot.reload;
            if (!simulation->ent_alive(petal_slot.ent_id)) {
                if (++petal_slot.reload >= SERVER_TIME(PETAL_DATA[slot.id].reload)) {
                    Entity &petal = simulation->alloc_petal(slot.id);
                    petal.set_x(player.x);
                    petal.set_y(player.y);
                    petal.set_parent(player.id);
                    petal.set_team(player.parent);
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
        camera.set_loadout_reloads(i, min * 255 / max);
    }
    player.rotation_count = rotation_pos;
    player.rotation_angle += REAL_TIME(2.5);

    camera.set_experience(player.score);
    player.set_face_flags(BIT_SHIFT(BIT_AT(player.input, 0), 0) | BIT_SHIFT(BIT_AT(player.input, 1), 1));
    //player.rotation_angle = fmod(player.rotation_angle, 2 * M_PI);
}

void tick_petal_behavior(Simulation *sim) {
    for (uint32_t i = 0; i < sim->active_entities.length; ++i) {
        Entity &ent = sim->get_ent(sim->active_entities[i]);
        if (ent.has_component(kFlower) && !ent.pending_delete) petal_behavior(sim, ent);
        else if (ent.has_component(kPetal) && !sim->ent_alive(ent.parent)) sim->request_delete(ent.id);
    }
}