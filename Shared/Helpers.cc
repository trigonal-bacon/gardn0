#include <Shared/Helpers.hh>

EntityId::EntityId() {
}

EntityId::EntityId(uint16_t id, uint16_t hash) : id(id), hash(hash) {
}

uint8_t EntityId::null() {
    return id == 0;
}

void EntityId::operator=(EntityId o) {
    id = o.id;
    hash = o.hash;
}

uint8_t EntityId::operator==(EntityId o) {
    return id == o.id && hash == o.hash;
}
void EntityId::operator=(uint32_t v) {
    id = hash = 0;
}