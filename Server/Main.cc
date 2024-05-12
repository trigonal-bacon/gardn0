#include <Shared/Helpers.hh>
#include <iostream>
#include <Shared/Simulation.hh>
#include <Server/Server.hh>

int main() {
    /*
    uint8_t packet[1024];
    Writer w(static_cast<uint8_t *>(packet));
    w.write_uint8(123);
    w.write_uint32(427335);
    w.write_int32(-4586543);
    w.write_float(634.1);
    w.write_float(-53233.2);
    w.write_float(-0.002);
    Reader r(static_cast<uint8_t *>(packet));
    std::cout << r.read_uint8() << ' ' << r.read_uint32() << ' ' << r.read_int32() << ' ' << r.read_float() << ' ' << r.read_float() << ' ' << r.read_float() << '\n';
    */
    global_server.run();
    
    return 0;
}