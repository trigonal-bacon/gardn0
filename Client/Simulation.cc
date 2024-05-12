#include <Shared/Simulation.hh>

#include <cmath>

void Simulation::tick() {
    pre_tick();
    //lerpy lerp;
}

void Simulation::tick_lerp(double dt) {
    double _amt = 1 - (pow(1 - 0.2, dt * 60 / 1000));
    for (uint32_t i = 0; i < active_entities.length; ++i) {
        Entity &ent = get_ent(active_entities[i]);
        double amt = ent.touched ? _amt : 1;
        if (ent.has_component(kPhysics)) {
            LERP(ent.lerp_x, ent.x, amt);
            LERP(ent.lerp_y, ent.y, amt);
            LERP(ent.lerp_angle, ent.angle, amt);
            LERP(ent.lerp_radius, ent.radius, amt);
            if (ent.deletion_tick > 0) LERP(ent.lerp_deletion_tick, 5, _amt);
        }
        if (ent.has_component(kCamera)) {
            LERP(ent.lerp_camera_x, ent.camera_x, amt);
            LERP(ent.lerp_camera_y, ent.camera_y, amt);
            LERP(ent.lerp_fov, ent.fov, amt);
        }
        if (ent.has_component(kFlower)) {
            LERP(ent.lerp_eye_x, cosf(ent.eye_angle)*3, amt);
            LERP(ent.lerp_eye_y, sinf(ent.eye_angle)*3, amt);
        }
        if (ent.has_component(kHealth)) {
            LERP(ent.lerp_health, ent.health, amt);
            LERP(ent.lerp_max_health, ent.max_health, amt);
        }
        ent.touched = 1;
    }
}

void Simulation::post_tick() {
    for (uint32_t i = 0; i < pending_delete.length; ++i) {
        //guarantee entity exists
        assert(ent_exists(pending_delete[i]));
        Entity &ent = get_ent(pending_delete[i]);
        delete_ent(pending_delete[i]);
    }
    pending_delete.clear();
}