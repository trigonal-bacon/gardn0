#include <Client/Game.hh>

#include <Client/Render/RenderEntity.hh>

#include <iostream>

Game *gardn = nullptr;

Game::Game() {
    ui::g_window = &window;
    ui::g_renderer = &renderer;
    ui::g_focused = &window;
    ui::g_input = &input;
    gardn = this;
    socket.connect("ws://localhost:9001");
    /*
    window.add_child(new ui::HContainer{
        new ui::VContainer{
            new ui::StaticLabel{{"this is a text, ", 30}},
            new ui::StaticLabel{{"this is another text", 30}}
        },
        new ui::StaticLabel{{"this is a text, ", 30}},
        new ui::StaticLabel{{"this is another text", 30}}
    });
    */
    window.add_child(
        ui::SetRenderCondition(
            [&](){ return !alive() && !in_game; },
            new ui::VContainer{
                new ui::StaticLabel{{"Gardn.funny", 60}},
                new ui::StaticSpace(0,40),
                ui::SetRenderCondition(
                    [&](){ return !simulation_ready; },
                    new ui::StaticLabel{{"Loading...", 45}}
                ),
                ui::SetRenderCondition(
                    [&](){ return simulation_ready; },
                    new ui::VContainer{
                        ui::AddTheme(
                            ui::VisualData::ButtonBackground(0xfff34856),
                            new ui::Button(120, 36, new ui::StaticLabel{{"Enter Game", 20}}, [&](){
                                if (!alive()) {
                                    uint8_t packet[16];
                                    Writer writer(static_cast<uint8_t *>(packet));
                                    writer.write_uint8(kServerbound::kClientSpawn);
                                    socket.send(writer.packet, writer.at - writer.packet);
                                    in_game = 1;
                                }
                            })
                        ),
                        new ui::StaticSpace(0,15),
                        new ui::DynamicLabel{[&](char *buf){
                            if (!simulation_ready) return;
                            Entity &camera = simulation.get_ent(camera_id);
                            sprintf(buf, "You will spawn at level %u%s", get_level_from_xp(camera.experience), camera.loadout_ids[0] != PetalId::kNone ? " with:":"");
                        }, 16},             
                        new ui::StaticSpace(0,5),
                        (new ui::HContainer{
                            new ui::TitleScreenLoadout(0),
                            new ui::TitleScreenLoadout(1),
                            new ui::TitleScreenLoadout(2),
                            new ui::TitleScreenLoadout(3),
                            new ui::TitleScreenLoadout(4),
                            new ui::TitleScreenLoadout(5),
                            new ui::TitleScreenLoadout(6),
                            new ui::TitleScreenLoadout(7),
                            new ui::TitleScreenLoadout(8),
                            new ui::TitleScreenLoadout(9),
                            new ui::TitleScreenLoadout(10),
                            new ui::TitleScreenLoadout(11),
                            new ui::TitleScreenLoadout(12),
                            new ui::TitleScreenLoadout(13),
                            new ui::TitleScreenLoadout(14),
                            new ui::TitleScreenLoadout(15),
                        })->pad(0,10)
                    }
                ),
            }
        )
    );
    window.add_child(
        ui::SetRenderCondition(
            [&](){ return !alive() && in_game; },
            new ui::VContainer{
                new ui::StaticLabel{{"You Died", 40}},
                new ui::StaticSpace(0,20),
                new ui::DeathFlower(),
                new ui::StaticSpace(0,20),
                ui::AddTheme(
                    ui::VisualData::ButtonBackground(0xfff34856),
                    new ui::Button(120, 36, new ui::StaticLabel{{"Continue", 20}}, [&](){
                        in_game = 0;
                    })
                )
            }
        )
    );
    window.add_child(
        ui::in_game_loadout_init()
    );
    window.add_child(
        ui::Justify<-1,1>(
            ui::SetRenderCondition(
                [&](){ return alive(); },
                new ui::LevelBar()
            )
        )
    );
    window.add_child(
        ui::Justify<-1,1>(
            ui::SetRenderCondition(
                [&](){ return !in_game; },
                (new ui::HContainer{
                    ui::AddTheme(
                        ui::VisualData::ButtonBackground(0xff5648a6),
                        new ui::Button(120, 30, new ui::StaticLabel{{"Settings", 18}}, [&](){
                            if (ui::panel_open == ui::PanelOpen::kSettings) ui::panel_open = ui::PanelOpen::kNone;
                            else ui::panel_open = ui::PanelOpen::kSettings;
                        })
                    ),
                    ui::AddTheme(
                        ui::VisualData::ButtonBackground(0xff5648a6),
                        new ui::Button(120, 30, new ui::StaticLabel{{"Mob Gallery", 18}}, [&](){
                            if (ui::panel_open == ui::PanelOpen::kMobGallery) ui::panel_open = ui::PanelOpen::kNone;
                            else ui::panel_open = ui::PanelOpen::kMobGallery;
                        })
                    )
                })->pad(10,10)
            )
        )
    );
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
    renderer.context.reset();

    if (simulation_ready && (ui_circle_large > 0)) render_game();

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
    cache_slot_count = camera.loadout_count;
    if (alive()) for (uint32_t i = 0; i < cache_slot_count + MAX_SLOT_COUNT; ++i) cache_loadout[i] = camera.loadout_ids[i];
    renderer.translate(renderer.width / 2, renderer.height / 2);
    renderer.scale(scale * camera.lerp_fov);
    renderer.translate(-camera.lerp_camera_x, -camera.lerp_camera_y);
    if (alive()) {
        Entity &player = simulation.get_ent(camera.player);
        if (player.damage_flash > 0.1) {
            renderer.translate(frand() * 5 - 2.5, frand() * 5 - 2.5);
        }
    }
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
    {
        RenderContext context(&renderer);
        //circle thing for transitioning
        float dir = in_game ? 1 : -1;
        ui_circle_large = fclamp(ui_circle_large + dir * delta * 1.5, 0, 1100);
        if (ui_circle_large > 0 && ui_circle_large < 1100) {
            renderer.set_stroke(0xffffffff);
            renderer.set_line_width(10 * scale);
            renderer.begin_path();
            renderer.arc(renderer.width / 2, renderer.height / 2, ui_circle_large * scale);
            renderer.stroke();
            renderer.begin_path();
            renderer.rect(0, 0, renderer.width, renderer.height);
            renderer.reverse_arc(renderer.width / 2, renderer.height / 2, ui_circle_large * scale);
            renderer.clip();
        }
        if (ui_circle_large < 1100) {
            renderer.set_fill(0xff1ea761);
            renderer.fill_rect(0,0,renderer.width,renderer.height);
        }
    }
    ui::g_frame_dt = delta;
    ui::Element *prev_focused = ui::g_focused;
    ui::g_scale = scale;
    window.refactor();
    window.render(renderer);
    window.on_poll_events();
    ui::g_focused->emit_event();
    if (prev_focused != ui::g_focused) prev_focused->emit_event();
    ui::loadout_poll_input();
}

void Game::send_inputs() {
    uint8_t packet[1024];
    Writer writer(static_cast<uint8_t *>(packet));
    if (alive()) {
        writer.write_uint8(kServerbound::kClientInput);
        //float x = input.keys_pressed.contains('D') - input.keys_pressed.contains('A');
        //float y = input.keys_pressed.contains('S') - input.keys_pressed.contains('W');
        float x = (input.mouse_x - renderer.width / 2) / scale;
        float y = (input.mouse_y - renderer.height / 2) / scale;
        writer.write_float(x);
        writer.write_float(y);
        uint8_t attack = input.keys_pressed.contains('\x20') || BIT_AT(input.mouse_buttons_state, 0);
        uint8_t defend = input.keys_pressed.contains('\x10') || BIT_AT(input.mouse_buttons_state, 1);
        writer.write_uint8(attack | (defend << 1));
        socket.send(writer.packet, writer.at - writer.packet);
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