#pragma once

#include <stdint.h>
#include <assert.h>

#ifdef CLIENT_SIDE
#define CLIENT_ONLY(...) __VA_ARGS__
#else
#define CLIENT_ONLY(...)
#endif

#ifdef SERVER_SIDE
#define SERVER_ONLY(...) __VA_ARGS__
#else
#define SERVER_ONLY(...)
#endif

#ifdef DEBUG
#define DEBUG_ONLY(...) __VA_ARGS__
#else
#define DEBUG_ONLY(...)
#endif

class EntityId {
public:
    uint16_t id = 0;
    uint16_t hash = 0;
    EntityId();
    EntityId(uint16_t, uint16_t);
    bool null();
    void operator=(const EntityId &);
    void operator=(uint32_t);
};

bool operator<(const EntityId &, const EntityId &);
bool operator==(const EntityId &, const EntityId &);

constexpr uint32_t bit_count(uint32_t v) {return 32 - __builtin_clz(v - 1); };
constexpr uint32_t bit_fill(uint32_t v) { return (1 << v) - 1; }; 
constexpr uint32_t div_round_up(uint32_t a, uint32_t b) { return (a + b - 1) / b; }
double frand();
float fclamp(float, float, float); 
float lerp(float, float, float);

#define LERP(result, from, amt) { result = lerp(result, from, amt); }

template<typename T, uint32_t capacity>
class StaticArray {
public:
    T values[capacity];
    uint32_t length;
    StaticArray() : length(0) {};
    T &operator[](uint32_t at) { return values[at]; };
    void push(T val) { DEBUG_ONLY(assert(length < capacity); ) values[length++] = val; };
    void clear() { length = 0;}
    int32_t index_of(T val) {
        for (uint32_t i = 0; i < length; ++i) if (values[i] == val) return i;
        return -1;
    };
};