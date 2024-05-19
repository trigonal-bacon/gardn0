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

LerpValue::LerpValue() : value(0), lerp_value(0) {}

LerpValue::LerpValue(float v) : value(v), lerp_value(v) {}

float LerpValue::operator=(float v) { value = v; return v; }
void LerpValue::lerp_step(float v) { lerp_value = lerp(lerp_value, value, v); }

LerpValue::operator float() const { return lerp_value; }

SeedGenerator::SeedGenerator(uint32_t s) : seed(s) {}

float SeedGenerator::next() {
    seed *= 167436543;
    seed += 5832385;
    seed *= (76372345 + seed);
    seed += 937323;
    return (seed % 65536) / 65536.0f;
}