#include <Client/Game.hh>

#include <Client/Render/RenderEntity.hh>

#include <iostream>

Game *gardn = nullptr;

Game::Game() {
    gardn = this;
    socket.connect("ws://localhost:9001");
}

uint8_t Game::alive() {
    return simulation_ready && simulation.ent_exists(camera_id) && simulation.ent_alive(simulation.get_ent(camera_id).player);
}

void Game::tick(double time) {
    curr_tick = time;
    delta = curr_tick - last_tick;
    renderer.reset_transform();
    simulation.tick();
    simulation.tick_lerp(delta);

    if (simulation_ready && alive()) render_game();

    render_ui();
    
    if (socket.ready) send_inputs();
    
    simulation.post_tick();
    input.keys_pressed_this_tick.clear();
    input.mouse_buttons_pressed = input.mouse_buttons_released = 0;
    last_tick = curr_tick;
    input.prev_mouse_x = input.mouse_x;
    input.prev_mouse_y = input.mouse_y;
}

void Game::render_game() {
    RenderContext context(&renderer);
    assert(simulation.ent_exists(camera_id));
    Entity &camera = simulation.get_ent(camera_id);
    renderer.translate(renderer.width / 2, renderer.height / 2);
    renderer.scale(scale * camera.lerp_fov);
    renderer.translate(-camera.lerp_camera_x, -camera.lerp_camera_y);
    renderer.set_fill(0xff03a824);
    renderer.begin_path();
    renderer.fill_rect(0, 0, ARENA_WIDTH, ARENA_HEIGHT);
    for (uint32_t i = 0; i < simulation.active_entities.length; ++i) {
        Entity &ent = simulation.get_ent(simulation.active_entities[i]);
        if (ent.has_component(kFlower)) {
            RenderContext context(&renderer);
            renderer.translate(ent.lerp_x, ent.lerp_y);
            render_flower(ent, renderer);
        }
    }
}

void Game::render_ui() {

}

void Game::send_inputs() {
    uint8_t packet[1024];
    Writer writer(static_cast<uint8_t *>(packet));
    if (alive()) {
        writer.write_uint8(Serverbound::kClientInput);
        float x = input.keys_pressed.contains('D') - input.keys_pressed.contains('A');
        float y = input.keys_pressed.contains('S') - input.keys_pressed.contains('W');
        writer.write_float(x);
        writer.write_float(y);
        socket.send(writer.packet, writer.at - writer.packet);
    }
    else {
        if (input.keys_pressed_this_tick.contains('Q')) {
            //SPAWN
            writer.write_uint8(Serverbound::kClientSpawn);
            socket.send(writer.packet, writer.at - writer.packet);
        }
    }
}

void Game::on_message(uint8_t *ptr, uint32_t len) {
    Reader reader(ptr);
    switch(reader.read_uint8()) {
        case Clientbound::kClientUpdate: {
            simulation_ready = 1;
            camera_id = reader.read_entid();
            EntityId curr_id = reader.read_entid();
            while(1) {
                if (curr_id.null()) break;
                assert(simulation.ent_exists(curr_id));
                Entity &ent = simulation.get_ent(curr_id);
                simulation.request_delete(curr_id);
                curr_id = reader.read_entid();
            }
            curr_id = reader.read_entid();
            assert(!curr_id.null());
            while(1) {
                if (curr_id.null()) break;
                uint8_t create = reader.read_uint8();
                if (create) simulation.force_alloc_ent(curr_id);
                assert(simulation.ent_exists(curr_id));
                Entity &ent = simulation.get_ent(curr_id);
                ent.read(&reader);
                curr_id = reader.read_entid();
            }
            //std::cout << '\n';
            break;
        }
        default:
            break;
    }
}