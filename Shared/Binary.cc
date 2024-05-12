#include <Shared/Binary.hh>

Writer::Writer(uint8_t *buf) : at(buf), packet(buf) {}

void Writer::write_uint8(uint8_t v) {
    *at = v;
    ++at;
}

void Writer::write_uint32(uint32_t v) {
    while (v > 127)
    {
        write_uint8((v & 127) | 128);
        v >>= 7;
    }
    write_uint8(v);
}

void Writer::write_int32(int32_t v) {
    uint32_t sign = v < 0;
    if (sign) v *= -1;
    v = (v << 1) | sign;
    write_uint32(v);
}

void Writer::write_float(float v) {
    write_int32(v * 1024);
}

void Writer::write_entid(EntityId &id) {
    write_uint32(id.id);
    if (id.id) write_uint32(id.hash);
}

Reader::Reader(uint8_t const *buf) : at(buf), packet(buf) {}

uint8_t Reader::read_uint8() {
    return *at++;
}

uint32_t Reader::read_uint32() {
    uint32_t ret = 0;
    for (uint32_t i = 0; i < 5; ++i) {
        uint8_t o = read_uint8();
        ret |= ((o & 127) << (i * 7));
        if (o <= 127) break;
    }
    return ret;
}

int32_t Reader::read_int32() {
    uint32_t r = read_uint32();
    uint32_t s = r & 1;
    int32_t ret = r >> 1;
    if (s) ret *= -1;
    return ret;
}

float Reader::read_float() {
    return read_int32() / 1024.0;
}

EntityId Reader::read_entid() {
    uint16_t id = read_uint32();
    uint16_t hash = id ? read_uint32() : 0;
    return {id, hash};
}