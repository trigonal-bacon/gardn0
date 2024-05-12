#include <Server/Client.hh>

#include <Server/Server.hh>

void Client::init() {
    Entity &ent = global_server.simulation.alloc_ent();
    ent.add_component(kCamera);
    ent.set_fov(1.0f);
    ent.set_camera_x(frand() * ARENA_WIDTH);
    ent.set_camera_y(frand() * ARENA_HEIGHT);
    camera = ent.id;
}

void Client::remove() {
    if (global_server.simulation.ent_exists(camera)) {
        if (global_server.simulation.ent_exists(global_server.simulation.get_ent(camera).player))
            global_server.simulation.request_delete(global_server.simulation.get_ent(camera).player);
        global_server.simulation.request_delete(camera);
    }
    std::cout << "deleting client " << this << "\n";
}

uint8_t Client::alive() {
    return global_server.simulation.ent_exists(camera) 
    && global_server.simulation.ent_exists(global_server.simulation.get_ent(camera).player);
}