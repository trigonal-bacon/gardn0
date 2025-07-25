#include <Shared/Simulation.hh>

#include <Server/Server.hh>
#include <Server/Client.hh>
#include <Server/Process/Process.hh>

#include <cmath>
#include <iostream>
#include <set>
#include <vector>

Entity &_alloc_mob(Simulation *sim, uint8_t mob_id) {
    Entity &mob = sim->alloc_ent();
    mob.add_component(kPhysics);
    //default pos
    mob.set_x(frand() * ARENA_WIDTH);
    mob.set_y(frand() * ARENA_HEIGHT);
    mob.set_angle(frand() * 2 * M_PI);
    mob.set_radius(MOB_DATA[mob_id].radius);
    mob.friction = DEFAULT_FRICTION;
    mob.add_component(kRelations);
    mob.set_team(NULL_ENTITY);
    mob.add_component(kHealth);
    mob.set_max_health(MOB_DATA[mob_id].health);
    mob.set_health(MOB_DATA[mob_id].health);
    mob.damage = MOB_DATA[mob_id].damage;
    mob.add_component(kMob);
    mob.set_mob_id(mob_id);
    mob.add_component(kScore);
    mob.set_score(MOB_DATA[mob_id].xp * 2); //double since killing = half
    mob.set_name({MOB_DATA[mob_id].name});
    if (mob_id == MobId::kRock) mob.set_radius(15 + frand() * 25);
    else if (mob_id == MobId::kBoulder) mob.set_radius(40 + frand() * 40);
    else if (mob_id == MobId::kCactus) mob.set_radius(30 + frand() * 40);
    mob.mass = mob.radius / 20 + 3;
    return mob;
}

Entity &Simulation::alloc_mob(uint8_t mob_id) {
    if (mob_id == MobId::kCentipede || mob_id == MobId::kEvilCentipede) {
        Entity *head = &_alloc_mob(this, mob_id);
        head->add_component(kSegmented);
        Entity &ret = *head;
        ret.set_is_head(1);
        for (uint32_t i = 0; i < 9; ++i) {
            Entity *segment = &_alloc_mob(this, mob_id);
            segment->add_component(kSegmented);
            segment->head_node = head->id;
            segment->set_angle(head->angle + frand() * 0.1 - 0.05);
            segment->set_x(head->x - (head->radius + segment->radius) * cosf(segment->angle));
            segment->set_y(head->y - (head->radius + segment->radius) * sinf(segment->angle));
            head = segment;
        }
        return ret;
    }
    else if (mob_id == MobId::kAntHole) {
        Entity &hole = _alloc_mob(this, mob_id);
        hole.no_friendly_collision = 1;
        for (uint32_t i = 0; i < 3; ++i) {
            Entity &spawn = _alloc_mob(this, MobId::kBabyAnt);
            float angle = frand() * 2 * M_PI;
            float dist = sqrtf(frand()) * 100;
            spawn.set_x(hole.x + cosf(angle) * dist);
            spawn.set_y(hole.y + sinf(angle) * dist);
            spawn.set_parent(hole.id);
        }
        for (uint32_t i = 0; i < 2; ++i) {
            Entity &spawn = _alloc_mob(this, MobId::kWorkerAnt);
            float angle = frand() * 2 * M_PI;
            float dist = sqrtf(frand()) * 100;
            spawn.set_x(hole.x + cosf(angle) * dist);
            spawn.set_y(hole.y + sinf(angle) * dist);
            spawn.set_parent(hole.id);
        }
        return hole;
    }
    else return _alloc_mob(this, mob_id);
}

Entity &Simulation::alloc_petal(uint8_t petal_id) {
    Entity &petal = alloc_ent();
    petal.add_component(kPhysics);
    petal.set_radius(PETAL_DATA[petal_id].radius);
    petal.friction = DEFAULT_FRICTION;
    petal.mass = 0.2;
    petal.add_component(kRelations);
    petal.add_component(kPetal);
    petal.set_petal_id(petal_id);
    petal.add_component(kHealth);
    petal.set_max_health(PETAL_DATA[petal_id].health);
    petal.set_health(PETAL_DATA[petal_id].health);
    petal.damage = PETAL_DATA[petal_id].damage;
    petal.effect_delay = 0;
    petal.poison.define(REAL_TIME(PETAL_DATA[petal_id].extras.poison_damage / PETAL_DATA[petal_id].extras.poison_time), SERVER_TIME(PETAL_DATA[petal_id].extras.poison_time));
    petal.no_friendly_collision = 1;
    return petal;
}

Entity &Simulation::alloc_player(Entity &camera) {
    Entity &player = alloc_ent();
    player.add_component(kPhysics);
    player.set_x(camera.camera_x);
    player.set_y(camera.camera_y);
    player.set_radius(25);
    player.friction = DEFAULT_FRICTION;
    player.add_component(kFlower);
    camera.set_player(player.id);
    player.add_component(kRelations);
    player.set_parent(camera.id);
    player.set_team(camera.id);
    player.add_component(kHealth);
    player.set_health(100);
    player.set_max_health(100);
    player.damage = 25;
    player.immunity_ticks = SERVER_TIME(2);
    player.add_component(kScore);
    player.set_score(camera.experience / 2);
    player.no_friendly_collision = 1;
    return player;
}

void Simulation::tick() {
    if (frand() < 0.01) {
        Entity &mob = alloc_mob(MobId::kAntHole);
    }
    pre_tick();
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        ent.set_damaged(0); //very ugly but whatever, will make component vectors later
        if (ent.has_component(kPhysics)) spatial_hash.insert(ent);
    }
    spatial_hash.collide();
    tick_petal_behavior(this);
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        if (ent.has_component(kMob) && !ent.pending_delete) tick_mob_ai(this, ent);
    }
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        if (ent.pending_delete) continue;
        if (ent.has_component(kSegmented)) tick_centipede_behavior(this, ent);
        if (ent.has_component(kDrop)) tick_drop_behavior(this, ent);
        if (ent.has_component(kPhysics)) tick_entity_motion(this, ent);
        if (ent.has_component(kHealth)) tick_health_behavior(this, ent);
    }
    post_tick();
}

static void _ent_on_delete(Simulation *simulation, Entity &ent) {
    if (ent.team == NULL_ENTITY && ent.has_component(kMob)) {
        //spawn DROPS
        uint32_t count = 0;
        uint8_t drops[6];
        struct MobData &data = MOB_DATA[ent.mob_id];
        for (uint32_t i = 0; i < 6; ++i) {
            if (data.drops[i].id == 0) break;
            if (frand() < data.drops[i].chance) drops[count++] = data.drops[i].id;
        }
        if (count == 0) return;
        for (uint32_t i = 0; i < count; ++i) {
            Entity &drop = simulation->alloc_ent();
            drop.add_component(kPhysics);
            drop.set_x(ent.x);
            drop.set_y(ent.y);
            drop.set_radius(0);
            drop.set_angle(-M_PI);
            drop.friction = 0.4;
            drop.add_component(kRelations);
            drop.set_team(NULL_ENTITY);
            drop.add_component(kDrop);
            drop.set_drop_id(drops[i]);
            drop.despawn_ticks = 0;
            if (count > 1) {
                Vector v;
                v.unit_normal(2 * M_PI * i / count).set_magnitude(SERVER_TIME(PLAYER_ACCELERATION) * 0.4);
                drop.acceleration = v;
            }
            simulation->spatial_hash.insert(drop);
        }
    }
    else if (ent.has_component(kFlower)) {
        uint8_t petal_queue[10 + MAX_SLOT_COUNT * 2] = {0};
        Entity &camera = simulation->get_ent(ent.parent);
        for (uint32_t i = 0; i < camera.loadout_count + MAX_SLOT_COUNT; ++i) if (camera.loadout_ids[i] != PetalId::kBasic && frand() > 0.05) petal_queue[i] = camera.loadout_ids[i];
        for (uint32_t i = 0; i < 10; ++i) if (frand() > 0.05) camera.loadout_ids[camera.loadout_count + MAX_SLOT_COUNT + i] = camera.deleted_petals[i];
        std::sort(&petal_queue[0], &petal_queue[camera.loadout_count + MAX_SLOT_COUNT + 10],
        [](uint8_t a, uint8_t b){
            return (PETAL_DATA[a].rarity + (a != PetalId::kNone)) > (PETAL_DATA[b].rarity + (b != PetalId::kNone));
        });
        uint32_t count = 0;
        uint8_t drops[3];
        for (uint32_t i = 0; i < 3; ++i) {
            if (petal_queue[i] == PetalId::kNone) break;
            drops[count++] = petal_queue[i];
        }
        if (count == 0) return;
        for (uint32_t i = 0; i < count; ++i) {
            Entity &drop = simulation->alloc_ent();
            drop.add_component(kPhysics);
            drop.set_x(ent.x);
            drop.set_y(ent.y);
            drop.set_radius(0);
            drop.set_angle(-M_PI);
            drop.friction = 0.4;
            drop.add_component(kRelations);
            drop.set_team(NULL_ENTITY);
            drop.add_component(kDrop);
            drop.set_drop_id(drops[i]);
            drop.despawn_ticks = 0;
            if (count > 1) {
                Vector v;
                v.unit_normal(2 * M_PI * i / count).set_magnitude(SERVER_TIME(PLAYER_ACCELERATION) * 0.4);
                drop.acceleration = v;
            }
            simulation->spatial_hash.insert(drop);
        }
        for (uint32_t i = 0; i < camera.loadout_count + MAX_SLOT_COUNT; ++i) camera.set_loadout_ids(i, petal_queue[i + count]);
    }
}

void Simulation::post_tick() {
    //delete all pending deletes or advance deletion tick by one
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        assert(ent_exists(active_entities[i]));
        Entity &ent = get_ent(active_entities[i]);
        if (ent.pending_delete && ent.has_component(kPhysics)) {
            if (ent.deletion_tick == 0) _ent_on_delete(this, ent);
            ent.set_deletion_tick(ent.deletion_tick + 1);
            if (pending_delete.index_of(ent.id) == -1) pending_delete.push(ent.id);
        }
    }

    calculate_leaderboard();

    //for (client of clients) client.send_update();
    for (Client *client: global_server.clients) update_client(client);
    //send_state & reset all remaining active entities
    //reset state of all entities FIRST
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        assert(ent_exists(active_entities[i]));
        Entity &ent = get_ent(active_entities[i]);
        ent.reset_protocol_state();
    }
    for (uint32_t i = 0; i < pending_delete.length; ++i) {
        //guarantee entity exists
        assert(ent_exists(pending_delete[i]));
        Entity &ent = get_ent(pending_delete[i]);
        if (!ent.has_component(kPhysics) || ent.deletion_tick > 5) delete_ent(pending_delete[i]);
    }
    pending_delete.clear();
    spatial_hash.clear();
}

void Simulation::update_client(Client *client) {
    if (!ent_exists(client->camera)) return;
    std::set<EntityId> in_view;
    in_view.insert(client->camera);
    Entity &camera = get_ent(client->camera);
    if (ent_exists(camera.player)) {
        Entity &player = get_ent(camera.player);
        camera.set_camera_x(player.x);
        camera.set_camera_y(player.y);
        in_view.insert(player.id);
    }
    Writer writer(OUTGOING_PACKET);
    writer.write_uint8(kClientbound::kClientUpdate);
    writer.write_entid(client->camera);
    spatial_hash.query(camera.camera_x, camera.camera_y, 960 / camera.fov, 540 / camera.fov, [&](Simulation *sim, Entity &ent){
        in_view.insert(ent.id);
    });

    for (EntityId i: client->last_in_view) {
        if (!in_view.contains(i)) writer.write_entid(i);
    }
    writer.write_entid(NULL_ENTITY);
    //upcreates
    for (EntityId id: in_view) {
        assert(ent_exists(id));
        Entity &ent = get_ent(id);
        uint8_t create = !client->last_in_view.contains(id);
        writer.write_entid(id);
        writer.write_uint8(create);
        ent.write(&writer, create);
    }
    writer.write_entid(NULL_ENTITY);
    //scoreboard
    writer.write_uint32(leaderboard.length);
    if (leaderboard.length > 0) {
        for (uint32_t i = 0; i < leaderboard.length - 1; ++i) {
            writer.write_string(leaderboard[i].name);
            writer.write_entid(leaderboard[i].id);
            writer.write_float(leaderboard[i].score);
        }
        if (ent_exists(camera.player) && get_ent(camera.player).score <= leaderboard[leaderboard.length - 1].score) {
            Entity &player = get_ent(camera.player);
            writer.write_string(player.name);
            writer.write_entid(player.id);
            writer.write_float(player.score);
        }
        else {
            writer.write_string(leaderboard[leaderboard.length - 1].name);
            writer.write_entid(leaderboard[leaderboard.length - 1].id);
            writer.write_float(leaderboard[leaderboard.length - 1].score);
        }
    }
    //set client->last_in_view
    client->last_in_view.clear();
    for (EntityId i: in_view) client->last_in_view.insert(i);

    std::string_view message(reinterpret_cast<char const *>(writer.packet), writer.at - writer.packet);
    client->ws->send(message, uWS::OpCode::BINARY, 0);
}

void Simulation::calculate_leaderboard() {
    std::vector<EntityId> players;
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        //guarantee entity exists
        assert(ent_exists(active_entities[i]));
        Entity &ent = get_ent(active_entities[i]);
        if (ent.has_component(kFlower) && ent.has_component(kScore)) players.push_back(active_entities[i]);
    }
    uint32_t num = players.size();
    if (num > 10) num = 10;
    leaderboard.clear();
    if (num == 0) return;
    for (uint32_t i = 0; i < num; ++i) {
        float max_score = 0;
        uint32_t max_ind = 0;
        for (uint32_t j = 0; j < players.size(); ++j) {
            EntityId &id = players[j];
            float sc = get_ent(id).score;
            if (sc > max_score) { max_score = sc; max_ind = j; }
        }
        leaderboard.push({get_ent(players[max_ind]).name, players[max_ind], max_score});
        players[max_ind] = players[players.size() - 1];
        players.pop_back();
    }
}