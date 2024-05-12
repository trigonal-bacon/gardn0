#pragma once

#include <Shared/Helpers.hh>

#include <stdint.h>

enum Clientbound {
    kClientUpdate
};

enum Serverbound {
    kClientInput,
    kClientSpawn
};

class Writer {
public:
    uint8_t *packet;
    uint8_t *at;
    Writer(uint8_t *);
    void write_uint8(uint8_t);
    void write_uint32(uint32_t);
    void write_int32(int32_t);
    void write_float(float);
    void write_entid(EntityId &);
};

class Reader {
public:
    uint8_t const *packet;
    uint8_t const *at;
    Reader(uint8_t const *);
    uint8_t read_uint8();
    uint32_t read_uint32();
    int32_t read_int32();
    float read_float();
    EntityId read_entid();

/*
    void read_uint8(uint8_t &);
    void read_uint32(uint32_t &);
    void read_int32(int32_t &);
    void read_float(float &);
    void read_entid(EntityId &);
    */
};