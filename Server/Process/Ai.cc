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

void tick_mob_ai(Simulation *simulation, Entity &entity) {
    entity.ai_ticks_to_next_action++;// -= entity.ai_ticks_to_next_action > 0;
    switch(entity.mob_id) {
        case kMobId::kBabyAnt:
            tick_baby_ant_ai(simulation, entity);
            break;
    }
}