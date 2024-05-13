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
    uint32_t alpha = (uint32_t)(camera.lerp_fov * 255 * 0.2) << 24;
    {
        RenderContext context(&renderer);
        renderer.reset_transform();
        renderer.set_fill(0xff1ea761);
        renderer.fill_rect(0,0,renderer.width,renderer.height);
        renderer.set_fill(alpha);
        renderer.fill_rect(0,0,renderer.width,renderer.height);
    }
    {
        RenderContext context(&renderer);
        renderer.set_fill(0xff1ea761);
        renderer.fill_rect(0,0,ARENA_WIDTH,ARENA_HEIGHT);
        renderer.set_stroke(alpha);
        renderer.set_line_width(1);
        float _scale = camera.lerp_fov * scale;
        float leftX = camera.lerp_camera_x - renderer.width / (2 * _scale);
        float rightX = camera.lerp_camera_x + renderer.width / (2 * _scale);
        float topY = camera.lerp_camera_y - renderer.height / (2 * _scale);
        float bottomY = camera.lerp_camera_y + renderer.height / (2 * _scale);
        float newLeftX = ceilf(leftX / 50) * 50;
        float newTopY = ceilf(topY / 50) * 50;
        renderer.begin_path();
        for (; newLeftX < rightX; newLeftX += 50)
        {
            renderer.move_to(newLeftX, topY);
            renderer.line_to(newLeftX, bottomY);
        }
        for (; newTopY < bottomY; newTopY += 50)
        {
            renderer.move_to(leftX, newTopY);
            renderer.line_to(rightX, newTopY);
        }
        renderer.stroke();
    }
    //render arena
    /*

            rr_renderer_fill_rect(this->renderer, 0, 0, renderer.width,
                                  renderer.height);
            rr_renderer_set_fill(this->renderer, alpha);
            rr_renderer_fill_rect(this->renderer, 0, 0, renderer.width,
                                  renderer.height);
            rr_renderer_context_state_free(this->renderer, &state2);
            rr_renderer_context_state_init(this->renderer, &state2);

            struct rr_component_arena *arena =
                rr_simulation_get_arena(this->simulation, 1);
            rr_renderer_begin_path(this->renderer);
            rr_renderer_arc(this->renderer, 0, 0, arena->radius);
            rr_renderer_set_fill(this->renderer, 0xff1ea761);
            rr_renderer_fill(this->renderer);
            rr_renderer_clip(this->renderer);

            rr_renderer_set_stroke(this->renderer, alpha);
            rr_renderer_set_line_width(this->renderer, 1);

            float scale = camera.lerp_camera_fov * renderer.scale;
            float leftX = camera.lerp_camera_x - renderer.width / (2 * scale);
            float rightX = camera.lerp_camera_x + renderer.width / (2 * scale);
            float topY = camera.lerp_camera_y - renderer.height / (2 * scale);
            float bottomY = camera.lerp_camera_y + renderer.height / (2 * scale);
            float newLeftX = ceilf(leftX / 50) * 50;
            float newTopY = ceilf(topY / 50) * 50;
            for (; newLeftX < rightX; newLeftX += 50)
            {
                rr_renderer_begin_path(this->renderer);
                rr_renderer_move_to(this->renderer, newLeftX, topY);
                rr_renderer_line_to(this->renderer, newLeftX, bottomY);
                rr_renderer_stroke(this->renderer);
            }
            for (; newTopY < bottomY; newTopY += 50)
            {
                rr_renderer_begin_path(this->renderer);
                rr_renderer_move_to(this->renderer, leftX, newTopY);
                rr_renderer_line_to(this->renderer, rightX, newTopY);
                rr_renderer_stroke(this->renderer);
            }
            rr_renderer_set_global_alpha(this->renderer, 1);

            rr_renderer_context_state_free(this->renderer, &state2);
            */
    for (uint32_t i = 0; i < simulation.active_entities.length; ++i) {
        Entity &ent = simulation.get_ent(simulation.active_entities[i]);
        if (ent.has_component(kHealth)) {
            RenderContext context(&renderer);
            renderer.translate(ent.lerp_x, ent.lerp_y);
            render_health(ent, renderer);
        }
    }
    for (uint32_t i = 0; i < simulation.active_entities.length; ++i) {
        Entity &ent = simulation.get_ent(simulation.active_entities[i]);
        if (ent.has_component(kDrop)) {
            RenderContext context(&renderer);
            renderer.translate(ent.lerp_x, ent.lerp_y);
            renderer.rotate(ent.lerp_angle);
            render_drop(ent, renderer);
        }
    }
    for (uint32_t i = 0; i < simulation.active_entities.length; ++i) {
        Entity &ent = simulation.get_ent(simulation.active_entities[i]);
        if (ent.has_component(kPetal)) {
            RenderContext context(&renderer);
            renderer.translate(ent.lerp_x, ent.lerp_y);
            renderer.rotate(ent.lerp_angle);
            render_petal(ent, renderer);
        }
    }
    for (uint32_t i = 0; i < simulation.active_entities.length; ++i) {
        Entity &ent = simulation.get_ent(simulation.active_entities[i]);
        if (ent.has_component(kMob)) {
            RenderContext context(&renderer);
            renderer.translate(ent.lerp_x, ent.lerp_y);
            renderer.rotate(ent.lerp_angle);
            render_mob(ent, renderer);
        }
    }
    for (uint32_t i = 0; i < simulation.active_entities.length; ++i) {
        Entity &ent = simulation.get_ent(simulation.active_entities[i]);
        if (ent.has_component(kFlower)) {
            RenderContext context(&renderer);
            renderer.translate(ent.lerp_x, ent.lerp_y);
            renderer.rotate(ent.lerp_angle);
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
        writer.write_uint8(kServerbound::kClientInput);
        //float x = input.keys_pressed.contains('D') - input.keys_pressed.contains('A');
        //float y = input.keys_pressed.contains('S') - input.keys_pressed.contains('W');
        float x = input.mouse_x - renderer.width / 2;
        float y = input.mouse_y - renderer.height / 2;
        writer.write_float(x);
        writer.write_float(y);
        uint8_t attack = input.keys_pressed.contains('\x20') || BIT_AT(input.mouse_buttons_state, 0);
        uint8_t defend = input.keys_pressed.contains('\x10') || BIT_AT(input.mouse_buttons_state, 1);
        writer.write_uint8(attack | (defend << 1));
        socket.send(writer.packet, writer.at - writer.packet);
    }
    else {
        if (input.keys_pressed_this_tick.contains('Q')) {
            //SPAWN
            writer.write_uint8(kServerbound::kClientSpawn);
            socket.send(writer.packet, writer.at - writer.packet);
        }
    }
}

void Game::on_message(uint8_t *ptr, uint32_t len) {
    Reader reader(ptr);
    switch(reader.read_uint8()) {
        case kClientbound::kClientUpdate: {
            simulation_ready = 1;
            camera_id = reader.read_entid();
            EntityId curr_id = reader.read_entid();
            while(1) {
                if (curr_id.null()) break;
                assert(simulation.ent_exists(curr_id));
                Entity &ent = simulation.get_ent(curr_id);
                simulation.delete_ent(curr_id);
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
            //simulation.post_tick();
            break;
        }
        default:
            break;
    }
}