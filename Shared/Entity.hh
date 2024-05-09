#pragma once

#include <Shared/Binary.hh>
#include <Shared/Helpers.hh>
#include <Shared/Vector.hh>

#include <stdint.h>

enum Components {
    kPhysics,
    kRelations,
    kCamera
};

#define PERFIELD \
SINGLE(x, float, kPhysics, 1) \
SINGLE(y, float, kPhysics, 2) \
SINGLE(radius, float, kPhysics, 3) \
SINGLE(angle, float, kPhysics, 4) \
SINGLE(deletionTick, uint8_t, kPhysics, 5) \
SINGLE(team, uint32_t, kRelations, 6) \
SINGLE(parent, uint32_t, kRelations, 7) \
SINGLE(cameraX, float, kCamera, 8) \
SINGLE(cameraY, float, kCamera, 9) \
SINGLE(FOV, float, kCamera, 10)

#ifdef SERVER
#define PERSVFIELD \
SINGLE(friction, float, =0) \
SINGLE(mass, float, =1) \
SINGLE(velocity, Vector, .set(0,0)) \
SINGLE(acceleration, Vector, .set(0,0))
#endif
class Entity {
public:
    Entity();
    void init();
    void reset();
    uint32_t components;
    uint32_t id;
#define SINGLE(name, type, component, num) type name;
PERFIELD
#undef SINGLE
#define SINGLE(name, type, component, num) uint8_t state_##name;
PERFIELD
#undef SINGLE
#ifdef SERVER
#define SINGLE(name, type, reset) type name;
PERSVFIELD
#undef SINGLE
void add_component(uint32_t);
uint8_t has_component(uint32_t);
void write(Writer *, uint8_t);
#define SINGLE(name, type, component, num) void set_##name(type);
PERFIELD
#undef SINGLE
#else
void read(Reader *);
#endif
};

#undef SINGLE