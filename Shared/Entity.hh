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

typedef EntityId entid;
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int32_t int32;
#define PERFIELD \
SINGLE(x, float, kPhysics, 1) \
SINGLE(y, float, kPhysics, 2) \
SINGLE(radius, float, kPhysics, 3) \
SINGLE(angle, float, kPhysics, 4) \
SINGLE(deletion_tick, uint8, kPhysics, 5) \
SINGLE(team, entid, kRelations, 6) \
SINGLE(parent, entid, kRelations, 7) \
SINGLE(camera_x, float, kCamera, 8) \
SINGLE(camera_y, float, kCamera, 9) \
SINGLE(fov, float, kCamera, 10)

#ifdef SERVER_SIDE
#define PER_EXTRA_FIELD \
SINGLE(pending_delete, uint8_t, =0) \
SINGLE(friction, float, =0) \
SINGLE(mass, float, =1) \
SINGLE(velocity, Vector, .set(0,0)) \
SINGLE(acceleration, Vector, .set(0,0))
#else
#define PER_EXTRA_FIELD \
SINGLE(pending_delete, uint8_t, =0)
#endif

#define NULL_ENTITY (0)
class Entity {
public:
    Entity();
    void init();
    void reset();
    void reset_protocol_state();
    uint32_t components;
    EntityId id;
#define SINGLE(name, type, component, num) type name;
PERFIELD
#undef SINGLE
#define SINGLE(name, type, component, num) uint8_t state_##name;
PERFIELD
#undef SINGLE
void add_component(uint32_t);
uint8_t has_component(uint32_t);
#define SINGLE(name, type, reset) type name;
PER_EXTRA_FIELD
#undef SINGLE
#ifdef SERVER_SIDE
void write(Writer *, uint8_t);
#define SINGLE(name, type, component, num) void set_##name(type);
PERFIELD
#undef SINGLE
#else
void read(Reader *);
#endif
};

#undef SINGLE