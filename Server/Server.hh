#pragma once

#include <Shared/Simulation.hh>
#include <App.h>

static const int max_buffer_size = 1024 * 1024;
class Server {
public:
    Simulation simulation;
    uWS::App *server;
    Server();
    void run();
    void tick();
};