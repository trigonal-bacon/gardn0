#pragma once

#include <Shared/Simulation.hh>

#include <App.h>

#include <set>

class Client;

static const int max_buffer_size = 1024 * 1024;

class Server {
public:
    Simulation simulation;
    uWS::App *server;
    std::set<Client *> sockets;
    Server();
    void run();
    void tick();
};

extern Server _server;