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

float angle_lerp(float start, float end, float t)
{
    t = fclamp(t, 0, 1);

    start = fmod(start, 2 * M_PI);
    end = fmod(end, 2 * M_PI);
    if (fabs(end - start) < M_PI)
        return (end - start) * t + start;
    else
    {
        if (end > start)
            start += 2 * M_PI;
        else
            end += 2 * M_PI;
        return fmod((end - start) * t + start + 2 * M_PI, 2 * M_PI);
    }
}

double frand() {
    return std::rand() / (double) RAND_MAX;
}

EntityId::EntityId() {
}

EntityId::EntityId(uint16_t id, uint16_t hash) : id(id), hash(hash) {
}

bool EntityId::null() const {
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

LerpValue::LerpValue() : value(0), lerp_value(0) {}

LerpValue::LerpValue(float v) : value(v), lerp_value(v) {}

float LerpValue::operator=(float v) { value = v; return v; }
void LerpValue::lerp_step(float v) { lerp_value = lerp(lerp_value, value, v); }

LerpValue::operator float() const { return lerp_value; }