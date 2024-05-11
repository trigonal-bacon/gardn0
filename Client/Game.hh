#pragma once

#include <Client/Render/Renderer.hh>
#include <Client/Input.hh>
#include <Client/Socket.hh>

#include <Shared/Simulation.hh>

class Game {
public:
    Simulation simulation;
    Renderer renderer;
    Input input;
    Socket socket;
    double curr_tick = 0;
    double last_tick = 0;
    double delta = 0;
    float scale = 1;
    uint8_t in_game = 0;
    Game();
    void tick(double);
    void on_message(uint8_t *, uint32_t);
};

extern Game *gardn;