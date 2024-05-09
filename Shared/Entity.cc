#include <Shared/Entity.hh>

Entity::Entity() {
    init();
}

void Entity::init() {
    #define SINGLE(name, type, component, num) name = 0; state_##name = 0;
    PERFIELD
    #undef SINGLE
}

void Entity::reset() {
    #define SINGLE(name, type, component, num) state_##name = 0;
    PERFIELD
    #undef SINGLE
#ifdef SERVER
    #define SINGLE(name, type, reset) name reset;
    PERSVFIELD
    #undef SINGLE
#endif
}

#ifdef SERVER
void Entity::add_component(uint32_t comp) {
    assert(!has_component(comp));
    components |= 1 << comp;
}
uint8_t Entity::has_component(uint32_t comp) {
    return (components >> comp) & 1;
}
#define SINGLE(name, type, component, num) void Entity::set_##name(type v) { name = v; state_##name = 1; }
PERFIELD
#undef SINGLE
#endif
void Entity::write(Writer *writer, uint8_t create) {
    writer->write_uint32_t(components);
#define SINGLE(name, type, component, num) if(has_component(component) && (create || state_##name)) writer->write_uint8_t(num); writer->write_##type(name);
PERFIELD
#undef SINGLE
    writer->write_uint8_t(0);
}
#undef SINGLE