#include <Server/Process/Process.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

#include <cmath>
#include <iostream>

void tick_baby_ant_ai(Simulation *simulation, Entity &entity) {
    switch(entity.ai_state) {
        case kIdle: {
            if (entity.ai_ticks_to_next_action >= SERVER_TIME(2)) {
                entity.set_angle(frand() * 2 * M_PI);
                entity.ai_ticks_to_next_action = 0;
                entity.ai_state = kIdleMoving;
                break;
            }
            break;
        }
        case kIdleMoving: {
            if (entity.ai_ticks_to_next_action > SERVER_TIME(1.5)) {
                entity.ai_ticks_to_next_action = 0;
                entity.ai_state = kIdle;
            }
            else if (entity.ai_ticks_to_next_action > SERVER_TIME(0.25)) {
                entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION / 2);
            }
            break;
        }
        default:
            entity.ai_state = kIdle;
            break;
    }
}

void tick_bee_ai(Simulation *simulation, Entity &entity) {
    switch(entity.ai_state) {
        case kIdle: {
            entity.bearing_angle = frand() * 2 * M_PI;
            entity.ai_state = kIdleMoving;
            break;
        }
        case kIdleMoving: {
            if (entity.ai_ticks_to_next_action > SERVER_TIME(5)) {
                entity.ai_ticks_to_next_action = 0;
                entity.bearing_angle = frand() * 2 * M_PI;
                entity.ai_state = kIdleMoving;
                break;
            }
            entity.set_angle(entity.bearing_angle + 0.75 * sinf(entity.ai_ticks_to_next_action * 0.05));
            entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION / 6);
            break;
        }
        default:
            entity.ai_state = kIdle;
            break;
    }
}

void tick_worker_ant_ai(Simulation *simulation, Entity &entity) {
    if (simulation->ent_alive(entity.target)) entity.ai_state = kAggro;
    switch(entity.ai_state) {
        case kIdle: {
            if (entity.ai_ticks_to_next_action >= SERVER_TIME(2)) {
                entity.set_angle(frand() * 2 * M_PI);
                entity.ai_ticks_to_next_action = 0;
                entity.ai_state = kIdleMoving;
                break;
            }
            break;
        }
        case kIdleMoving: {
            if (entity.ai_ticks_to_next_action > SERVER_TIME(1.5)) {
                entity.ai_ticks_to_next_action = 0;
                entity.ai_state = kIdle;
            }
            else if (entity.ai_ticks_to_next_action > SERVER_TIME(0.25)) {
                entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION / 2);
            }
            break;
        }
        case kAggro: {
            if (!simulation->ent_alive(entity.target)) {
                entity.ai_state = kIdle;
                entity.ai_ticks_to_next_action = 0;
                entity.target = NULL_ENTITY;
                break;
            }
            Entity &target = simulation->get_ent(entity.target);
            Vector diff(target.x - entity.x, target.y - entity.y);
            entity.set_angle(diff.angle());
            entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION);
            break;
        }
        default:
            entity.ai_state = kIdle;
            break;
    }
}

void tick_mob_ai(Simulation *simulation, Entity &entity) {
    entity.ai_ticks_to_next_action++;// -= entity.ai_ticks_to_next_action > 0;
    switch(entity.mob_id) {
        case kMobId::kBabyAnt:
            tick_baby_ant_ai(simulation, entity);
            break;
        case kMobId::kWorkerAnt:
            tick_worker_ant_ai(simulation, entity);
            break;
        case kMobId::kBee:
            tick_bee_ai(simulation, entity);
            break;
    }
}