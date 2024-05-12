#include <Shared/Helpers.hh>

#include <random>

float fclamp(float v, float s, float e) {
    if (!(v >= s)) return s;
    if (!(v <= e)) return e;
    return v;
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

bool EntityId::null() {
    return id == 0;
}

void EntityId::operator=(const EntityId &o) {
    id = o.id;
    hash = o.hash;
}


void EntityId::operator=(uint32_t v) {
    id = hash = 0;
}

bool operator<(const EntityId &a, const EntityId &b) {
    return (a.hash * 65536 + a.id) < (b.hash * 65536 + b.id);
}

bool operator==(const EntityId &a, const EntityId &b) {
    return a.id == b.id && a.hash == b.hash;
}