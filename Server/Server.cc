#include <Server/Server.hh>

#include <Server/Client.hh>

#include <chrono>
#include <thread>
#include <stdio.h>

Server _server;
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
            client->ws = ws;
            _server.sockets.insert(client);
        },
        .message = [](auto *ws, std::string_view message, uWS::OpCode opCode) {
			//std::cout << "client message recv\n";
            //ws->send(message, opCode, 0);
        },
        .dropped = [](auto *ws, std::string_view /*message*/, uWS::OpCode /*opCode*/) {
            ws->end();
            /* A message was dropped due to set maxBackpressure and closeOnBackpressureLimit limit */
        },
        .drain = [](auto */*ws*/) {
            /* Check ws->getBufferedAmount() here */
        },
        .close = [](auto *ws, int /*code*/, std::string_view /*message*/) {
            /* You may access ws->getUserData() here */
            Client *client = ws->getUserData();
            _server.sockets.erase(client);
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
        _server.simulation.tick();
        timespec_get(&te, TIME_UTC);

        double mss = ts.tv_sec * 1000 + ts.tv_nsec / 1000000.0;
        double mse = te.tv_sec * 1000 + te.tv_nsec / 1000000.0;
        //std::cout << "tick took " << (mse - mss) << "ms\n";

        //std::cout << "Broadcasting timestamp: " << millis << std::endl;

        //globalApp->publish("broadcast", std::string_view((char *) &millis, sizeof(millis)), uWS::OpCode::BINARY, false);

    }, 50, 50);
    app.run();
}