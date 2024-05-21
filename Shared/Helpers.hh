#pragma once

#include <stdint.h>
#include <assert.h>

#include <string>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

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

constexpr uint32_t bit_count(uint32_t v) {return 32 - __builtin_clz(v - 1); };
constexpr uint32_t bit_fill(uint32_t v) { return (1 << v) - 1; }; 
constexpr uint32_t div_round_up(uint32_t a, uint32_t b) { return (a + b - 1) / b; }
double frand();
float fclamp(float, float, float); 
float lerp(float, float, float);
float angle_lerp(float, float, float);

#define LERP(result, from, amt) { result = lerp(result, from, amt); }
#define ANGLE_LERP(result, from, amt) { result = angle_lerp(result, from, amt); }

#define BIT_AT(val, bit) (((val) >> (bit)) & 1)
#define BIT_SHIFT(bit, shift) ((bit) << (shift))

template<typename T, uint32_t capacity>
class StaticArray {
public:
    T values[capacity];
    uint32_t length;
    StaticArray() : length(0) {};
    T &operator[](uint32_t at) { return values[at]; };
    void push(T val) { DEBUG_ONLY(assert(length < capacity); ) values[length++] = val; };
    void clear() { length = 0;}
    int32_t index_of(T val) const {
        for (uint32_t i = 0; i < length; ++i) if (values[i] == val) return i;
        return -1;
    };
};

class LerpValue {
public:
    float value = 0;
    float lerp_value = 0;
    LerpValue();
    LerpValue(float);
    void lerp_step(float);
    float operator=(float);
    operator float() const;
};

class SeedGenerator {
    uint32_t seed;
public:
    SeedGenerator(uint32_t);
    float next();
};