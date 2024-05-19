#include <Shared/Entity.hh>

#include <Shared/StaticData.hh>
#include <iostream>

Entity::Entity() {
    init();
}

void Entity::init() {
    components = 0;
    #define _SINGLE(name, type) name = 0;
    #define _MULTIPLE(name, type, amt) for (uint32_t n = 0; n < amt; ++n) { name[n] = 0; }
    PERFIELD
    #undef _SINGLE
    #undef _MULTIPLE
    #define _SINGLE(name, type, reset) name reset;
    #define _MULTIPLE(name, type, amt, reset) for (uint32_t i = 0; i < amt; ++i) { name[i] reset; }
    PER_EXTRA_FIELD
    #undef _SINGLE
    #undef _MULTIPLE
    reset_protocol_state();
}

void Entity::reset_protocol_state() {
    #define _SINGLE(name, type) state_##name = 0;
    #define _MULTIPLE(name, type, amt) state_##name = 0; for (uint32_t n = 0; n < amt; ++n) { state_per_##name[n] = 0; }
    PERFIELD
    #undef _SINGLE
    #undef _MULTIPLE
}

void Entity::add_component(uint32_t comp) {
    assert(!has_component(comp));
    components |= 1 << comp;
}
uint8_t Entity::has_component(uint32_t comp) {
    return (components >> comp) & 1;
}

#ifdef SERVER_SIDE
#define _SINGLE(name, type) void Entity::set_##name(type v) { if (name == v) return; name = v; state_##name = 1; }
#define _MULTIPLE(name, type, amt) void Entity::set_##name(uint32_t i, type v) { if (name[i] == v) return; name[i] = v; state_##name = 1; state_per_##name[i] = 1; }
PERFIELD
#undef _SINGLE
#undef _MULTIPLE
void Entity::write(Writer *writer, uint8_t create) {
    writer->write_uint32(components);
#define _SINGLE(name, type) if(create || state_##name) { writer->write_uint8(k##name); writer->write_##type(name); }
#define _MULTIPLE(name, type, amt) if(create || state_##name) { \
    writer->write_uint8(k##name); \
    for (uint32_t n = 0; n < amt; ++n) { \
        if (create || state_per_##name[n]) { writer->write_uint8(n); writer->write_##type(name[n]); } \
        } \
    } \
    writer->write_uint8(255);
#define COMPONENT(name) if (has_component(k##name)) { FIELDS_##name }
PERCOMPONENT
#undef _SINGLE
#undef _MULTIPLE
    writer->write_uint8(kFieldCount);
}
#else

void Entity::read(Reader *reader) {
    prev_x = x;
    prev_y = y;
    components = reader->read_uint32();
    while(1) {
        switch(reader->read_uint8()) {
            case kFieldCount: { return; }
    #define _SINGLE(name, type) case k##name: { name = reader->read_##type(); state_##name = 1; break; }
    #define _MULTIPLE(name, type, amt) case k##name: { \
        while(1) { \
            uint8_t index = reader->read_uint8(); \
            if (index == 255) break; \
            name[index] = reader->read_##type(); \
            state_per_##name[index] = 1; \
        } \
        break; \
    }
    PERFIELD
    #undef _SINGLE  
    #undef _MULTIPLE      
        }
    }
}
#endif