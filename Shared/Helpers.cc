#include <Shared/Helpers.hh>

#include <random>

float fclamp(float v, float s, float e) {
    return v < s ? s : v > e ? e : v;
}

float lerp(float v, float e, float a) {
    a = fclamp(a, 0, 1);
    return v * (1 - a) + e * a;
}

double frand() {
    return std::rand() / (double) RAND_MAX;
}

EntityId::EntityId() {
}

EntityId::EntityId(uint16_t id, uint16_t hash) : id(id), hash(hash) {
}

uint8_t EntityId::null() {
    return id == 0;
}

void EntityId::operator=(EntityId o) {
    id = o.id;
    hash = o.hash;
}

uint8_t EntityId::operator==(EntityId o) {
    return id == o.id && hash == o.hash;
}
void EntityId::operator=(uint32_t v) {
    id = hash = 0;
}