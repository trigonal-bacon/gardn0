#include <Shared/Binary.hh>

Writer::Writer(uint8_t *buf) {
    at = packet = buf;
}

void Writer::write_uint8_t(uint8_t v) {
    *at = v;
    ++at;
}

void Writer::write_uint32_t(uint32_t v) {
    while (v > 127)
    {
        write_uint8_t((v & 127) | 128);
        v >>= 7;
    }
    write_uint8_t(v);
}

void Writer::write_int32_t(int32_t v) {
    uint32_t sign = v < 0;
    if (sign) v *= -1;
    v = (v << 1) | sign;
    write_uint32_t(v);
}

void Writer::write_float(float v) {
    write_int32_t(v * 1024);
}