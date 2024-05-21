#pragma once

#include <Shared/Binary.hh>
#include <Shared/EntityDef.hh>
#include <Shared/Helpers.hh>
#include <Shared/Vector.hh>
#include <Shared/StaticData.hh>

#include <stdint.h>
#include <string>

#define PERCOMPONENT \
COMPONENT(Physics) \
COMPONENT(Relations) \
COMPONENT(Camera) \
COMPONENT(Flower) \
COMPONENT(Petal) \
COMPONENT(Health) \
COMPONENT(Mob) \
COMPONENT(Drop) \
COMPONENT(Score) \
COMPONENT(Segmented)

enum Components {
    #define COMPONENT(name) k##name,
    PERCOMPONENT
    #undef COMPONENT
    kComponentCount
};

typedef EntityId entid;
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int32_t int32;
typedef std::string string;

#define FIELDS_Physics \
_SINGLE(x, float) \
_SINGLE(y, float) \
_SINGLE(radius, float) \
_SINGLE(angle, float) \
_SINGLE(deletion_tick, uint8)

#define FIELDS_Relations \
_SINGLE(team, entid) \
_SINGLE(parent, entid)

#define FIELDS_Camera \
_SINGLE(camera_x, float) \
_SINGLE(camera_y, float) \
_SINGLE(experience, float) \
_SINGLE(fov, float) \
_SINGLE(player, entid) \
_SINGLE(loadout_count, uint8) \
_MULTIPLE(loadout_ids, uint8, 2 * MAX_SLOT_COUNT) \
_MULTIPLE(loadout_reloads, uint8, MAX_SLOT_COUNT)

#define FIELDS_Flower \
_SINGLE(face_flags, uint8) \
_SINGLE(eye_angle, float)

#define FIELDS_Petal \
_SINGLE(petal_id, uint8)

#define FIELDS_Health \
_SINGLE(health, float) \
_SINGLE(max_health, float) \
_SINGLE(damaged, uint8)

#define FIELDS_Mob \
_SINGLE(mob_id, uint8)

#define FIELDS_Drop \
_SINGLE(drop_id, uint8)

#define FIELDS_Score \
_SINGLE(score, float) \
_SINGLE(name, string)

#define FIELDS_Segmented \
_SINGLE(is_head, uint8)

#define PERFIELD \
FIELDS_Physics FIELDS_Relations FIELDS_Camera FIELDS_Flower FIELDS_Petal FIELDS_Health FIELDS_Mob FIELDS_Drop FIELDS_Score FIELDS_Segmented

enum Fields {
    #define _SINGLE(name, type) k##name,
    #define _MULTIPLE(name, type, amt) k##name,
    PERFIELD
    #undef _SINGLE
    #undef _MULTIPLE
    kFieldCount
};

#ifdef SERVER_SIDE
#define PER_EXTRA_FIELD \
_SINGLE(pending_delete, uint8_t, =0) \
_SINGLE(input, uint8_t, =0) \
_SINGLE(detached, uint8_t, =0) \
_SINGLE(friction, float, =0) \
_SINGLE(mass, float, =1) \
_SINGLE(velocity, Vector, .set(0,0)) \
_SINGLE(collision_velocity, Vector, .set(0,0)) \
_SINGLE(acceleration, Vector, .set(0,0)) \
_SINGLE(rotation_count, uint32_t, =1) \
_SINGLE(rotation_angle, float, =0) \
_SINGLE(damage, float, =0) \
_SINGLE(despawn_ticks, uint32_t, =0) \
_SINGLE(ai_state, uint8_t, =kMobAiState::kIdle) \
_SINGLE(ai_ticks_to_next_action, uint32_t, =0) \
_MULTIPLE(loadout, LoadoutSlot, MAX_SLOT_COUNT, .reset()) \
_SINGLE(target, EntityId, =NULL_ENTITY) \
_SINGLE(head_node, EntityId, =NULL_ENTITY) \
_SINGLE(immunity_ticks, uint32_t, =0) \
_MULTIPLE(deleted_petals, uint8_t, 10, =PetalId::kNone) \
_SINGLE(bearing_angle, float, =0) \
_SINGLE(effect_delay, uint32_t, =0) \
_SINGLE(applied_poison, AppliedPoison, .reset()) \
_SINGLE(poison, PoisonDefinition, .define(0,0))
#else
#define PER_EXTRA_FIELD \
_SINGLE(pending_delete, uint8_t, =0) \
_SINGLE(touched, uint8_t, =0) \
_SINGLE(prev_x, float, =0) \
_SINGLE(prev_y, float, =0) \
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
_SINGLE(lerp_max_health, float, =0) \
_SINGLE(healthbar_opacity, float, =0) \
_SINGLE(animation_tick, float, =0) \
_SINGLE(damage_flash, float, =0)
#endif

class Entity {
public:
    Entity();
    void init();
    void reset_protocol_state();
    uint32_t components;
    EntityId id;
#define _SINGLE(name, type) type name;
#define _MULTIPLE(name, type, amt) type name[amt];
PERFIELD
#undef _SINGLE
#undef _MULTIPLE
#define _SINGLE(name, type) uint8_t state_##name;
#define _MULTIPLE(name, type, amt) uint8_t state_##name; uint8_t state_per_##name[amt];
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
    #define _SINGLE(name, type) void set_##name(type const);
    #define _MULTIPLE(name, type, amt) void set_##name(uint32_t, type const);
    PERFIELD
    #undef _SINGLE
    #undef _MULTIPLE
#else
    void read(Reader *);
#endif
};

#undef _SINGLE