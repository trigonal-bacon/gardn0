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
    if (petal.petal_id == PetalId::kWing && petal.effect_delay > SERVER_TIME(1.5)) {
        petal.effect_delay = 0;
    } 
    Vector move_to;
    float mag = (BIT_AT(player.input, 0)) ? 150 : (BIT_AT(player.input, 1)) ? 50 : 75;
    if (petal.petal_id == PetalId::kWing && BIT_AT(player.input, 0)) mag = 150 + 50 + 50 * sinf(M_PI * 2 * petal.effect_delay / SERVER_TIME(1.5));
    float angle = M_PI * 2 * rot_pos / player.rotation_count + player.rotation_angle;
    move_to.unit_normal(angle).set_magnitude(mag);
    move_to.x += player.x - petal.x - player.velocity.x;
    move_to.y += player.y - petal.y - player.velocity.y;
    if (PETAL_DATA[petal.petal_id].clump_radius != 0) {
        Vector clump_vector;
        float angle_addition = player.rotation_angle * 0.75 + 2 * M_PI * secondary_pos / PETAL_DATA[petal.petal_id].count;
        clump_vector.unit_normal(angle_addition).set_magnitude(PETAL_DATA[petal.petal_id].clump_radius);
        angle += angle_addition;
        move_to += clump_vector;
    }
    move_to *= 0.5;
    //if (move_to.magnitude() > 25) move_to.normalize().set_magnitude(25);
    petal.acceleration = move_to;
    if (petal.velocity.magnitude() > 25 / (1 - petal.friction)) petal.velocity.set_magnitude(25 / (1 - petal.friction));
    if (petal.petal_id == PetalId::kWing) petal.set_angle(fmod(petal.angle + REAL_TIME(10), M_PI * 2));
    else if (petal.petal_id == PetalId::kBeetleEgg) petal.set_angle(0);
    else if (petal.petal_id == PetalId::kMissile) petal.set_angle(angle);
    else petal.set_angle(fmod(petal.angle + REAL_TIME(2), M_PI * 2));
}

static void player_behavior(Simulation *simulation, Entity &player) {
    //player guaranteed to be ALIVE
    assert(simulation->ent_exists(player.parent));
    Entity &camera = simulation->get_ent(player.parent);
    camera.set_experience(player.score);
    uint32_t prev_slot_count = camera.loadout_count;
    uint32_t lvl = get_level_from_xp(player.score);
    uint32_t curr_slot_count = lvl / 15 + 5;
    if (curr_slot_count > MAX_SLOT_COUNT) curr_slot_count = MAX_SLOT_COUNT;
    if (curr_slot_count > prev_slot_count) {
        for (; prev_slot_count < curr_slot_count; ++prev_slot_count) {
            camera.loadout[prev_slot_count].reset();
            camera.loadout[prev_slot_count].id = camera.loadout_ids[prev_slot_count];
        }
        camera.set_loadout_count(curr_slot_count);
    }
    float ratio = player.health / player.max_health;
    float max_health_adj = 100 + fclamp((lvl - 1) * 50 / 44, 0, 50);
    player.set_max_health(max_health_adj);
    player.set_health(ratio * max_health_adj);
    uint32_t rotation_pos = 0;
    for (uint32_t i = 0; i < camera.loadout_count; ++i) {
        LoadoutSlot &slot = camera.loadout[i];
        camera.set_loadout_ids(i, slot.id);
        if (slot.id == 0) continue;
        if (slot.id == PetalId::kLeaf) inflict_heal(simulation, player, REAL_TIME(1));
        uint32_t max = SERVER_TIME(PETAL_DATA[slot.id].reload);
        uint32_t min = max;
        for (uint32_t j = 0; j < PETAL_DATA[slot.id].count; ++j) {
            LoadoutPetal &petal_slot = slot.petals[j];
            if (!simulation->ent_alive(petal_slot.ent_id)) {
                if (petal_slot.reload < min) min = petal_slot.reload;
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
                //spawn eggs here...
                ++petal.effect_delay;
                if (petal.petal_id == PetalId::kEgg && petal.effect_delay >= SERVER_TIME(PETAL_DATA[PetalId::kEgg].secondary_reload)) {
                    Entity &summon = simulation->alloc_mob(MobId::kSoldierAnt);
                    summon.set_team(player.parent);
                    summon.set_parent(player.id);
                    summon.set_x(petal.x);
                    summon.set_y(petal.y);
                    simulation->request_delete(petal.id);
                    petal_slot.ent_id = summon.id;
                    continue;
                }
                if (petal.petal_id == PetalId::kBeetleEgg && petal.effect_delay >= SERVER_TIME(PETAL_DATA[PetalId::kBeetleEgg].secondary_reload)) {
                    Entity &summon = simulation->alloc_mob(MobId::kBeetle);
                    summon.set_team(player.parent);
                    summon.set_parent(player.id);
                    summon.set_x(petal.x);
                    summon.set_y(petal.y);
                    simulation->request_delete(petal.id);
                    petal_slot.ent_id = summon.id;
                    continue;
                }
                if (petal.petal_id == PetalId::kMissile && petal.effect_delay >= SERVER_TIME(PETAL_DATA[PetalId::kMissile].secondary_reload) && (BIT_AT(player.input, 0))) {
                    petal.detached = 1;
                    petal.effect_delay = SERVER_TIME(3);
                    petal.velocity.unit_normal(petal.angle).set_magnitude(PLAYER_ACCELERATION * 4 / petal.friction);
                    petal_slot.ent_id = NULL_ENTITY;
                    continue;
                }
                if (petal.petal_id == PetalId::kBubble && petal.effect_delay >= SERVER_TIME(PETAL_DATA[PetalId::kBubble].secondary_reload) && (BIT_AT(player.input, 1))) {
                    petal.detached = 1;
                    Vector delta(player.x - petal.x, player.y - petal.y);
                    player.acceleration += delta.set_magnitude(10 * PLAYER_ACCELERATION);
                    simulation->request_delete(petal.id);
                    //petal.effect_delay = SERVER_TIME(3);
                    //petal.velocity.unit_normal(petal.angle).set_magnitude(PLAYER_ACCELERATION * 4 / petal.friction);
                    //petal_slot.ent_id = NULL_ENTITY;
                    continue;
                }
                if (!petal.has_component(kMob) && petal.has_component(kPetal)) tick_petal(simulation, petal, rotation_pos, j);
                else --rotation_pos;
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
    if (player.applied_poison.ticks_left > 0) player.set_face_flags(BIT_SHIFT(1, 1) | BIT_SHIFT(1, 2));
    //player.rotation_angle = fmod(player.rotation_angle, 2 * M_PI);
}
static void tick_detached_petal_behavior(Simulation *simulation, Entity &petal) {
    if (--petal.effect_delay == 0) return simulation->request_delete(petal.id);
    if (petal.petal_id == PetalId::kMissile) {
        petal.acceleration.unit_normal(petal.angle).set_magnitude(PLAYER_ACCELERATION * 2);
    }
}
void tick_petal_behavior(Simulation *sim) {
    for (uint32_t i = 0; i < sim->active_entities.length; ++i) {
        Entity &ent = sim->get_ent(sim->active_entities[i]);
        if (ent.has_component(kFlower) && !ent.pending_delete) player_behavior(sim, ent);
        else if (ent.has_component(kPetal) && ent.detached) tick_detached_petal_behavior(sim, ent);
        else if (ent.has_component(kPetal) && !sim->ent_alive(ent.parent)) sim->request_delete(ent.id);
    }
}