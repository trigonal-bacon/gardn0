#pragma once

#include <Client/Render/Renderer.hh>
#include <Client/Input.hh>
#include <Client/Socket.hh>
#include <Client/Ui/Ui.hh>

#include <Shared/Simulation.hh>

class Game {
public:
    Simulation simulation;
    Renderer renderer;
    Input input;
    Socket socket;
    ui::Window window;
    EntityId camera_id;
    double curr_tick = 0;
    double last_tick = 0;
    double delta = 0;
    float scale = 1;
    float ui_circle_large = 0;
    uint8_t in_game = 0;
    uint8_t simulation_ready = 0;
    uint8_t cache_slot_count = 5;
    uint8_t cache_loadout[2 * MAX_SLOT_COUNT] = {0};
    Game();
    uint8_t alive();
    void tick(double);
    void render_game();
    void render_ui();
    void send_inputs();
    void on_message(uint8_t *, uint32_t);
};

extern Game *gardn;