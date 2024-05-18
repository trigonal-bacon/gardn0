#include <Server/Server.hh>

#include <Server/Client.hh>

#include <Shared/StaticData.hh>

#include <chrono>
#include <cmath>
#include <thread>
#include <stdio.h>

uint8_t OUTGOING_PACKET[max_buffer_size] = {0};
Server global_server;

Server::Server() : simulation() { }

void Server::run() {

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App app = uWS::App({
        /* There are example certificates in uWebSockets.js repo */
	    .key_file_name = "misc/key.pem",
	    .cert_file_name = "misc/cert.pem",
	    .passphrase = "1234"
	}).ws<Client>("/*", {
        /* Settings */
        .compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR_4KB | uWS::DEDICATED_DECOMPRESSOR),
        .maxPayloadLength = 1024 * 1024,
        .idleTimeout = 16,
        .maxBackpressure = 100 * 1024 * 1024,
        .closeOnBackpressureLimit = false,
        .resetIdleTimeoutOnSend = false,
        .sendPingsAutomatically = true,
        /* Handlers */
        .upgrade = nullptr,
        .open = [](auto *ws) {
			std::cout << "client connection\n";
            Client *client = ws->getUserData();
            client->init();
            client->ws = ws;
            global_server.clients.insert(client);
        },
        .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
            uint8_t const *data = reinterpret_cast<uint8_t const *>(message.data());
            Reader reader(data);
            Client *client = ws->getUserData();
            switch(reader.read_uint8()) {
                case kServerbound::kClientInput: {
                    if (!client->alive()) break;
                    Entity &camera = global_server.simulation.get_ent(client->camera);
                    Entity &player = global_server.simulation.get_ent(camera.player);
                    float x = reader.read_float();
                    float y = reader.read_float();
                    if (x == 0 && y == 0) player.acceleration.set(0,0);
                    else {
                        if (fabsf(x) > 5e3 || fabsf(y) > 5e3) break;
                        Vector accel(x,y);
                        float m = accel.magnitude();
                        if (m > 200) accel.normalize().set_magnitude(PLAYER_ACCELERATION);
                        else accel.normalize().set_magnitude(m / 200 * PLAYER_ACCELERATION);
                        player.acceleration = accel;
                    }
                    player.input = reader.read_uint8() & 3;
                    break;
                }
                case kServerbound::kClientSpawn: {
                    std::cout << "client spawn\n";
                    if (client->alive()) break;
                    Entity &camera = global_server.simulation.get_ent(client->camera);
                    Entity &player = global_server.simulation.alloc_player(camera);
                    player.set_score(camera.experience / 2);
                    uint32_t slots = 5 + get_level_from_xp(player.score) / 15;
                    if (slots > MAX_SLOT_COUNT) slots = MAX_SLOT_COUNT;
                    camera.set_loadout_count(slots);
                    for (uint32_t i = 0; i < slots; ++i) {
                        camera.loadout[i].reset();
                        camera.loadout[i].id = camera.loadout_ids[i] ? camera.loadout_ids[i] : kBasic;
                    }
                    for (uint32_t i = slots + MAX_SLOT_COUNT; i < 2 * MAX_SLOT_COUNT; ++i) camera.set_loadout_ids(i, kNone);
                    break;
                }
                case kServerbound::kPetalSwap: {
                    if (!client->alive()) break;
                    Entity &camera = global_server.simulation.get_ent(client->camera);
                    uint8_t pos1 = reader.read_uint8();
                    uint8_t pos2 = reader.read_uint8();
                    if (pos1 >= MAX_SLOT_COUNT + camera.loadout_count || pos2 >= MAX_SLOT_COUNT + camera.loadout_count) break;
                    uint8_t id1 = camera.loadout_ids[pos1];
                    uint8_t id2 = camera.loadout_ids[pos2];
                    camera.set_loadout_ids(pos1, id2);
                    camera.set_loadout_ids(pos2, id1);
                    if (pos1 < camera.loadout_count) {
                        for (uint32_t i = 0; i < 3; ++i) {
                            EntityId &entid = camera.loadout[pos1].petals[i].ent_id;
                            if (global_server.simulation.ent_alive(entid)) global_server.simulation.request_delete(entid);
                        }
                        camera.loadout[pos1].reset();
                        camera.loadout[pos1].id = id2;
                    }
                    if (pos2 < camera.loadout_count) {
                        for (uint32_t i = 0; i < 3; ++i) {
                            EntityId &entid = camera.loadout[pos2].petals[i].ent_id;
                            if (global_server.simulation.ent_alive(entid)) global_server.simulation.request_delete(entid);
                        }
                        camera.loadout[pos2].reset();
                        camera.loadout[pos2].id = id1;
                    }
                    break;
                }
                case kServerbound::kPetalDelete: {
                    if (!client->alive()) break;
                    Entity &camera = global_server.simulation.get_ent(client->camera);
                    Entity &player = global_server.simulation.get_ent(camera.player);
                    uint8_t pos = reader.read_uint8();
                    if (pos >= camera.loadout_count + MAX_SLOT_COUNT) break;
                    uint8_t id = camera.loadout_ids[pos];
                    player.set_score(player.score + RARITY_SACRIFICE_XP[PETAL_DATA[id].rarity]);
                    if (pos < camera.loadout_count) {
                        for (uint32_t i = 0; i < 3; ++i) {
                            EntityId &entid = camera.loadout[pos].petals[i].ent_id;
                            if (global_server.simulation.ent_alive(entid)) global_server.simulation.request_delete(entid);
                        }
                        camera.loadout[pos].reset();
                    }
                    else camera.set_loadout_ids(pos, kNone);
                    //add to the deletion queue
                    if (id == kBasic) break;
                    for (uint32_t i = 0; i < 9; ++i) camera.deleted_petals[9 - i] = camera.deleted_petals[8 - i];
                    camera.deleted_petals[0] = id;
                    break;
                }
                default:
                    ws->end();
                    break;
            }
			//std::cout << "client message recv\n";
            //ws->send(message, opCode, 0);
        },
        .dropped = [](auto *ws, std::string_view /*message*/, uWS::OpCode /*opCode*/) {
            std::cout << "wtf?\n";
            ws->end();
            /* A message was dropped due to set maxBackpressure and closeOnBackpressureLimit limit */
        },
        .drain = [](auto */*ws*/) {
            assert(!1);
            /* Check ws->getBufferedAmount() here */
        },
        .close = [](auto *ws, int /*code*/, std::string_view /*message*/) {
            /* You may access ws->getUserData() here */
            std::cout << "client disconnection\n";
            Client *client = ws->getUserData();
            client->remove();
            global_server.clients.erase(client);
        }
    }).listen(9001, [](auto *listen_socket) {
        if (listen_socket) {
            std::cout << "Listening on port " << 9001 << std::endl;
        }
    });
    server = &app;
    struct us_loop_t *loop = (struct us_loop_t *) uWS::Loop::get();
    struct us_timer_t *delayTimer = us_create_timer(loop, 0, 0);


    us_timer_set(delayTimer, [](struct us_timer_t */*t*/) {

        struct timespec ts;
        struct timespec te;
        timespec_get(&ts, TIME_UTC);
        global_server.simulation.tick();
        timespec_get(&te, TIME_UTC);

        double mss = ts.tv_sec * 1000 + ts.tv_nsec / 1000000.0;
        double mse = te.tv_sec * 1000 + te.tv_nsec / 1000000.0;
        //std::cout << "tick took " << (mse - mss) << "ms\n";

        //std::cout << "Broadcasting timestamp: " << millis << std::endl;

        //globalApp->publish("broadcast", std::string_view((char *) &millis, sizeof(millis)), uWS::OpCode::BINARY, false);

    }, SERVER_DT * 1000, SERVER_DT * 1000);
    app.run();
}