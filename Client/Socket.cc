#include <Client/Socket.hh>
#include <Client/Game.hh>

#include <emscripten.h>

uint8_t INCOMING_PTR[1024 * 1024] = {0};

extern "C" {
    void on_message(uint8_t type, uint32_t len) {
        if (type == 0) gardn->socket.ready = 1;
        else if (type == 2) gardn->socket.ready = 0;
        else if (type == 1) gardn->on_message(static_cast<uint8_t *>(INCOMING_PTR), len);
    }
}

Socket::Socket() {}

void Socket::connect(char const *url) {
    EM_ASM({
        let string = UTF8ToString($1);
        (function() {
            let socket = Module.socket = new WebSocket(string);
            socket.binaryType = "arraybuffer";
            socket.onopen = function()
            {
                console.log("Connected");
                _on_message(0);
            };
            socket.onclose = function(a)
            {
                _on_message(2, a.code);
            };
            socket.onmessage = function(event)
            {
                HEAPU8.set(new Uint8Array(event.data), $0);
                _on_message(1, event.data.length);
            };
        })();
    }, INCOMING_PTR, url);
}

void Socket::send(uint8_t *ptr, uint32_t len) {
    if (ready == 0) return; 
    EM_ASM({
        if (Module.socket?.readyState == 1) {
            Module.socket.send(HEAPU8.subarray($0, $0+$1));
        }
    }, ptr, len);
}