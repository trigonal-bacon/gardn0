#include <Shared/Simulation.hh>

#include <Server/Server.hh>
#include <Server/Client.hh>
#include <Server/Process/Process.hh>

#include <iostream>
#include <set>

void Simulation::tick() {
    if (frand() < 0.1) {
        Entity &mob = alloc_ent();
        uint8_t mob_id = kBabyAnt;
        mob.add_component(kPhysics);
        mob.set_x(frand() * ARENA_WIDTH);
        mob.set_y(frand() * ARENA_HEIGHT);
        mob.set_angle(frand() * 2 * M_PI);
        mob.set_radius(MOB_DATA[mob_id].radius);
        mob.add_component(kRelations);
        mob.set_team(NULL_ENTITY);
        mob.add_component(kHealth);
        mob.set_max_health(MOB_DATA[mob_id].health);
        mob.set_health(MOB_DATA[mob_id].health);
        mob.damage = MOB_DATA[mob_id].damage;
        mob.add_component(kMob);
        mob.set_mob_id(mob_id);
    }
    pre_tick();
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        if (ent.has_component(kPhysics)) spatial_hash.insert(ent);
    }
    spatial_hash.collide();
    tick_petal_behavior(this);
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        if (ent.has_component(kPhysics)) tick_entity_motion(this, ent);
    }
    post_tick();
}

static void _ent_on_delete(Simulation *simulation, Entity &ent) {
    if (ent.team == NULL_ENTITY && ent.has_component(kMob)) {
        //spawn DROPS
        uint32_t count = 0;
        uint8_t drops[4];
        struct MobData &data = MOB_DATA[ent.mob_id];
        for (uint32_t i = 0; i < 4; ++i) {
            if (data.drops[i].id == 0) break;
            if (frand() < data.drops[i].chance) drops[count++] = data.drops[i].id;
        }
        if (count == 0) return;
        for (uint32_t i = 0; i < count; ++i) {
            Entity &drop = simulation->alloc_ent();
            drop.add_component(kPhysics);
            drop.set_x(ent.x);
            drop.set_y(ent.y);
            drop.set_radius(25);
            drop.set_angle(0);
            drop.friction = 0.2;
            drop.add_component(kRelations);
            drop.set_team(NULL_ENTITY);
            drop.add_component(kDrop);
            drop.set_drop_id(drops[i]);
            if (count > 1) {
                Vector v;
                v.unit_normal(2 * M_PI * i / count).set_magnitude(10);
                drop.velocity = v;
            }
        }
    }
}

void Simulation::post_tick() {
    //reset state of all entities FIRST
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

    //for (client of clients) client.send_update();
    for (Client *client: global_server.clients) update_client(client);
    //send_state & reset all remaining active entities
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
    //set client->last_in_view
    client->last_in_view.clear();
    for (EntityId i: in_view) client->last_in_view.insert(i);

    std::string_view message(reinterpret_cast<char const *>(writer.packet), writer.at - writer.packet);
    client->ws->send(message, uWS::OpCode::BINARY, 0);
}