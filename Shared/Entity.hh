#pragma once

#include <Shared/Binary.hh>
#include <Shared/Helpers.hh>
#include <Shared/Vector.hh>

#include <stdint.h>

#define PERCOMPONENT \
COMPONENT(Physics) \
COMPONENT(Relations) \
COMPONENT(Camera) \
COMPONENT(Flower)

enum Components {
#define COMPONENT(name) k##name,
PERCOMPONENT
#undef COMPONENT
};

typedef EntityId entid;
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int32_t int32;

#define FIELDS_Physics \
SINGLE(x, float, 1) \
SINGLE(y, float, 2) \
SINGLE(radius, float, 3) \
SINGLE(angle, float, 4) \
SINGLE(deletion_tick, uint8, 5)

#define FIELDS_Relations \
SINGLE(team, entid, 6) \
SINGLE(parent, entid, 7)

#define FIELDS_Camera \
SINGLE(camera_x, float, 8) \
SINGLE(camera_y, float, 9) \
SINGLE(fov, float, 10) \
SINGLE(player, entid, 11)

#define FIELDS_Flower \
SINGLE(face_flags, uint8, 12) \
SINGLE(eye_angle, float, 13)

#define PERFIELD \
FIELDS_Physics FIELDS_Relations FIELDS_Camera FIELDS_Flower

#ifdef SERVER_SIDE
#define PER_EXTRA_FIELD \
SINGLE(pending_delete, uint8_t, =0) \
SINGLE(friction, float, =0) \
SINGLE(mass, float, =1) \
SINGLE(velocity, Vector, .set(0,0)) \
SINGLE(collision_velocity, Vector, .set(0,0)) \
SINGLE(acceleration, Vector, .set(0,0))
#else
#define PER_EXTRA_FIELD \
SINGLE(pending_delete, uint8_t, =0) \
SINGLE(touched, uint8_t, =0) \
SINGLE(lerp_x, float, =0) \
SINGLE(lerp_y, float, =0) \
SINGLE(lerp_angle, float, =0) \
SINGLE(lerp_radius, float, =0) \
SINGLE(lerp_camera_x, float, =0) \
SINGLE(lerp_camera_y, float, =0) \
SINGLE(lerp_fov, float, =1) \
SINGLE(lerp_deletion_tick, float, =0) \
SINGLE(lerp_eye_x, float, =3) \
SINGLE(lerp_eye_y, float, =0) \
SINGLE(lerp_mouth, float, =0)
#endif

extern EntityId NULL_ENTITY;
class Entity {
public:
    Entity();
    void init();
    void reset_protocol_state();
    uint32_t components;
    EntityId id;
#define SINGLE(name, type, num) type name;
PERFIELD
#undef SINGLE
#define SINGLE(name, type, num) uint8_t state_##name;
PERFIELD
#undef SINGLE
void add_component(uint32_t);
uint8_t has_component(uint32_t);
#define SINGLE(name, type, reset) type name;
PER_EXTRA_FIELD
#undef SINGLE
#ifdef SERVER_SIDE
void write(Writer *, uint8_t);
#define SINGLE(name, type, num) void set_##name(type);
PERFIELD
#undef SINGLE
#else
void read(Reader *);
#endif
};

#undef SINGLE