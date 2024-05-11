#include <Client/Game.hh>

#include <iostream>

Game *gardn = nullptr;

Game::Game() {
    gardn = this;
    socket.connect("ws://localhost:9001");
}

void Game::tick(double time) {
    curr_tick = time;
    delta = curr_tick - last_tick;
    /*
    renderer.reset_transform();
    renderer.set_fill(0xffff0000);
    renderer.translate(renderer.width / 2, renderer.height / 2);
    renderer.begin_path();
    renderer.arc(0,0,10);
    renderer.fill();
    */
    /*
    if (socket.ready) {
        uint8_t packet[1024];
        Writer w(static_cast<uint8_t *>(packet));
        w.write_uint8(123);
        socket.send(w.packet, w.at - w.packet);
    }
    */
    
    input.keys_pressed_this_tick.clear();
    input.mouse_buttons_pressed = input.mouse_buttons_released = 0;
    last_tick = curr_tick;
    input.prev_mouse_x = input.mouse_x;
    input.prev_mouse_y = input.mouse_y;
}

void Game::on_message(uint8_t *ptr, uint32_t len) {
    Reader reader(ptr);
}