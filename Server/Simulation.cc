#include <Shared/Simulation.hh>

#include <Server/Server.hh>
#include <Server/Client.hh>
#include <Server/Process/Process.hh>

#include <iostream>
#include <set>

void Simulation::tick() {
    if (frand() < 0.00) {
        Entity &ent = alloc_ent();
        ent.add_component(kPhysics);
        ent.set_radius(frand() * 5 + 5);
        ent.set_x(frand() * ARENA_WIDTH);
        ent.set_y(frand() * ARENA_HEIGHT);
    }
    pre_tick();
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        if (ent.has_component(kPhysics)) spatial_hash.insert(ent);
    }
    spatial_hash.collide();
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        if (ent.has_component(kPhysics)) tick_entity_motion(this, ent);
    }
    post_tick();
}

void Simulation::post_tick() {
    //reset state of all entities FIRST
    //delete all pending deletes or advance deletion tick by one
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        assert(ent_exists(active_entities[i]));
        Entity &ent = get_ent(active_entities[i]);
        if (ent.pending_delete && ent.has_component(kPhysics)) {
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
    std::set<EntityId, std::less<>> in_view;
    in_view.insert(client->camera);
    Entity &camera = get_ent(client->camera);
    if (ent_exists(camera.player)) {
        Entity &player = get_ent(camera.player);
        camera.set_camera_x(player.x);
        camera.set_camera_y(player.y);
        in_view.insert(player.id);
    }
    Writer writer(OUTGOING_PACKET);
    writer.write_uint8(Clientbound::kClientUpdate);
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