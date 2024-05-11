#pragma once

#include <Shared/Helpers.hh>

#include <App.h>

class Client {
public:
    EntityId camera;
    uint8_t alive;
    uWS::WebSocket<false, true, Client> *ws;
    Client();
    ~Client();
    //void on_message();
};