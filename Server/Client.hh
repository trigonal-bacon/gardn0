#pragma once

#include <Shared/Helpers.hh>

#include <set>
#include <stdint.h>
#include <App.h>

class Client {
public:
    EntityId camera;
    std::set<EntityId> last_in_view;
    uWS::WebSocket<false, true, Client> *ws;
    void init();
    void remove();
    uint8_t alive();
    //void on_message();
};