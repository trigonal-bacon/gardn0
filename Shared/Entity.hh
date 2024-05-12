#pragma once

#include <Shared/Binary.hh>
#include <Shared/Helpers.hh>
#include <Shared/Vector.hh>

#include <stdint.h>

#define PERCOMPONENT \
COMPONENT(Physics) \
COMPONENT(Relations) \
COMPONENT(Camera) \
COMPONENT(Flower) \
COMPONENT(Petal) \
COMPONENT(Health) \
COMPONENT(Mob) \
COMPONENT(Drop)

enum Components {
#define COMPONENT(name) k##name,
PERCOMPONENT
#undef COMPONENT
kComponentCount
};

class LoadoutPetal {
public:
    uint32_t reload;
    EntityId ent_id;
};

class LoadoutSlot {
public:
    uint8_t id;
    LoadoutPetal petals[3];
    LoadoutSlot();
    void reset();
};

typedef EntityId entid;
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int32_t int32;

#define FIELDS_Physics \
_SINGLE(x, float, 1) \
_SINGLE(y, float, 2) \
_SINGLE(radius, float, 3) \
_SINGLE(angle, float, 4) \
_SINGLE(deletion_tick, uint8, 5)

#define FIELDS_Relations \
_SINGLE(team, entid, 6) \
_SINGLE(parent, entid, 7)

#define FIELDS_Camera \
_SINGLE(camera_x, float, 8) \
_SINGLE(camera_y, float, 9) \
_SINGLE(fov, float, 10) \
_SINGLE(player, entid, 11) \
_SINGLE(loadout_count, uint8, 12) \
_MULTIPLE(loadout_ids, uint8, 5, 13) \
_MULTIPLE(loadout_reloads, uint8, 5, 14)

#define FIELDS_Flower \
_SINGLE(face_flags, uint8, 15) \
_SINGLE(eye_angle, float, 16)

#define FIELDS_Petal \
_SINGLE(petal_id, uint8, 17)

#define FIELDS_Health \
_SINGLE(health, float, 18) \
_SINGLE(max_health, float, 19)

#define FIELDS_Mob \
_SINGLE(mob_id, uint8, 20)

#define FIELDS_Drop \
_SINGLE(drop_id, uint8, 21)

#define PERFIELD \
FIELDS_Physics FIELDS_Relations FIELDS_Camera FIELDS_Flower FIELDS_Petal FIELDS_Health FIELDS_Mob FIELDS_Drop

#ifdef SERVER_SIDE
#define PER_EXTRA_FIELD \
_SINGLE(pending_delete, uint8_t, =0) \
_SINGLE(input, uint8_t, =0) \
_SINGLE(friction, float, =0) \
_SINGLE(mass, float, =1) \
_SINGLE(velocity, Vector, .set(0,0)) \
_SINGLE(collision_velocity, Vector, .set(0,0)) \
_SINGLE(acceleration, Vector, .set(0,0)) \
_SINGLE(rotation_count, uint32_t, =1) \
_SINGLE(rotation_angle, float, =0) \
_SINGLE(damage, float, =0) \
_SINGLE(despawn_ticks, uint32_t, =0) \
_SINGLE(ai_ticks_to_next_action, uint32_t, =0) \
_MULTIPLE(loadout, LoadoutSlot, 5, .reset())
#else
#define PER_EXTRA_FIELD \
_SINGLE(pending_delete, uint8_t, =0) \
_SINGLE(touched, uint8_t, =0) \
_SINGLE(lerp_x, float, =0) \
_SINGLE(lerp_y, float, =0) \
_SINGLE(lerp_angle, float, =0) \
_SINGLE(lerp_radius, float, =0) \
_SINGLE(lerp_camera_x, float, =0) \
_SINGLE(lerp_camera_y, float, =0) \
_SINGLE(lerp_fov, float, =1) \
_SINGLE(lerp_deletion_tick, float, =0) \
_SINGLE(lerp_eye_x, float, =3) \
_SINGLE(lerp_eye_y, float, =0) \
_SINGLE(lerp_mouth, float, =0) \
_SINGLE(lerp_health, float, =0) \
_SINGLE(lerp_max_health, float, =0)
#endif

extern EntityId NULL_ENTITY;
class Entity {
public:
    Entity();
    void init();
    void reset_protocol_state();
    uint32_t components;
    EntityId id;
#define _SINGLE(name, type, num) type name;
#define _MULTIPLE(name, type, amt, num) type name[amt] = {0};
PERFIELD
#undef _SINGLE
#undef _MULTIPLE
#define _SINGLE(name, type, num) uint8_t state_##name;
#define _MULTIPLE(name, type, amt, num) uint8_t state_##name; uint8_t state_per_##name[amt];
PERFIELD
#undef _SINGLE
#undef _MULTIPLE
void add_component(uint32_t);
uint8_t has_component(uint32_t);
#define _SINGLE(name, type, reset) type name;
#define _MULTIPLE(name, type, amt, reset) type name[amt];
PER_EXTRA_FIELD
#undef _SINGLE
#undef _MULTIPLE
#ifdef SERVER_SIDE
void write(Writer *, uint8_t);
#define _SINGLE(name, type, num) void set_##name(type);
#define _MULTIPLE(name, type, amt, num) void set_##name(type, uint32_t);
PERFIELD
#undef _SINGLE
#undef _MULTIPLE
#else
void read(Reader *);
#endif
};

#undef _SINGLE