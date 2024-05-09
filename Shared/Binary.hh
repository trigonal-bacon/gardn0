#pragma once

#include <stdint.h>

class Writer {
public:
    uint8_t *packet;
    uint8_t *at;
    Writer(uint8_t *);
    void write_uint8_t(uint8_t);
    void write_uint32_t(uint32_t);
    void write_int32_t(int32_t);
    void write_float(float);
};

class Reader {
public:
    uint8_t *packet;
    uint8_t *at;
    Reader(uint8_t *);
    uint8_t read_uint8_t();
    uint32_t read_uint32_t();
    int32_t read_int32_t();
    float read_float();
};