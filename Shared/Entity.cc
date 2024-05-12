#include <Shared/Entity.hh>

#include <iostream>

EntityId NULL_ENTITY(0, 0);

Entity::Entity() {
    init();
}

void Entity::init() {
    components = 0;
    #define SINGLE(name, type, num) name = 0; state_##name = 0;
    PERFIELD
    #undef SINGLE
    #define SINGLE(name, type, reset) name reset;
    PER_EXTRA_FIELD
    #undef SINGLE
}

void Entity::reset_protocol_state() {
    #define SINGLE(name, type, num) state_##name = 0;
    PERFIELD
    #undef SINGLE
}

void Entity::add_component(uint32_t comp) {
    assert(!has_component(comp));
    components |= 1 << comp;
}
uint8_t Entity::has_component(uint32_t comp) {
    return (components >> comp) & 1;
}

#ifdef SERVER_SIDE
#define SINGLE(name, type, num) void Entity::set_##name(type v) { name = v; state_##name = 1; }
PERFIELD
#undef SINGLE
void Entity::write(Writer *writer, uint8_t create) {
    writer->write_uint32(components);
#define SINGLE(name, type, num) if(create || state_##name) { writer->write_uint8(num); writer->write_##type(name); }
#define COMPONENT(name) if (has_component(k##name)) { FIELDS_##name }
PERCOMPONENT
#undef SINGLE
    writer->write_uint8(0);
}
#undef SINGLE
#else
void Entity::read(Reader *reader) {
    components = reader->read_uint32();
    while(1) {
        switch(reader->read_uint8()) {
            case 0: { return; }
    #define SINGLE(name, type, num) case num: { name = reader->read_##type(); state_##name = 1; break; }
    PERFIELD
    #undef SINGLE        
        }
    }
}
#endif