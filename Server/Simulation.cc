#include <Shared/Simulation.hh>

#include <Server/Process/Process.hh>

#include <iostream>

void Simulation::tick() {
    //Entity &ent = alloc_ent();
    //ent.add_component(kPhysics);
    pre_tick();
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        if (ent.has_component(kPhysics)) spatial_hash.insert(ent);
    }
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        if (ent.has_component(kPhysics)) tick_entity_motion(this, ent);
    }
    post_tick();
}

void Simulation::post_tick() {
    //reset state of all entities FIRST
    //delete all pending deletes or advance deletion tick by one
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        assert(ent_exists(active_entities[i]));
        Entity &ent = get_ent(active_entities[i]);
        if (ent.pending_delete && ent.has_component(kPhysics)) {
            ent.set_deletion_tick(ent.deletion_tick + 1);
        }
    }

    //for (client of clients) client.send_update();

    //send_state & reset all remaining active entities
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        assert(ent_exists(active_entities[i]));
        Entity &ent = get_ent(active_entities[i]);
        ent.reset_protocol_state();
    }
    for (uint32_t i = 0; i < pending_delete.length; ++i) {
        //guarantee entity exists
        assert(ent_exists(pending_delete[i]));
        Entity &ent = get_ent(pending_delete[i]);
        if (!ent.has_component(kPhysics) || ent.deletion_tick > 5) delete_ent(pending_delete[i]);
    }
    pending_delete.clear();
    spatial_hash.clear();
}