#include <Server/Process/Process.hh>

#include <Server/EntityFunctions.hh>
#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

#include <cmath>
#include <iostream>

static const float DETECTION_RANGE = 600;
void default_tick_idle(Simulation *simulation, Entity &entity) {
    if (entity.ai_ticks_to_next_action >= SERVER_TIME(2)) {
        entity.set_angle(frand() * 2 * M_PI);
        entity.ai_ticks_to_next_action = 0;
        entity.ai_state = kIdleMoving;
    }
}

void default_tick_idle_moving(Simulation *simulation, Entity &entity) {
    if (entity.ai_ticks_to_next_action > SERVER_TIME(2)) {
        entity.ai_ticks_to_next_action = 0;
        entity.ai_state = kIdle;
    }
    else if (entity.ai_ticks_to_next_action > SERVER_TIME(1.75)) entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION / 4);
    else if (entity.ai_ticks_to_next_action > SERVER_TIME(0.25)) entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION / 2);
}


void tick_baby_ant_ai(Simulation *simulation, Entity &entity) {
    switch(entity.ai_state) {
        case kIdle: {
            default_tick_idle(simulation, entity);
            break;
        }
        case kIdleMoving: {
            default_tick_idle_moving(simulation, entity);
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
    if (simulation->ent_alive(entity.target) && ai_state_is_passive(entity.ai_state)) entity.ai_state = kAggro;
    switch(entity.ai_state) {
        case kIdle: {
            default_tick_idle(simulation, entity);
            break;
        }
        case kIdleMoving: {
            default_tick_idle_moving(simulation, entity);
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

void tick_soldier_ant_ai(Simulation *simulation, Entity &entity) {
    if (!simulation->ent_alive(entity.target)) entity.target = find_nearest_enemy(simulation, entity, DETECTION_RANGE);
    if (simulation->ent_alive(entity.target) && ai_state_is_passive(entity.ai_state)) entity.ai_state = kAggro;
    switch(entity.ai_state) {
        case kIdle: {
            default_tick_idle(simulation, entity);
            break;
        }
        case kIdleMoving: {
            default_tick_idle_moving(simulation, entity);
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
            if (diff.magnitude() > DETECTION_RANGE + 300) {
                entity.ai_state = kIdle;
                entity.ai_ticks_to_next_action = 0;
                entity.target = NULL_ENTITY;
                break;
            }
            break;
        }
        default:
            entity.ai_state = kIdle;
            break;
    }
}

void tick_hornet_ai(Simulation *simulation, Entity &entity) {
    if (!simulation->ent_alive(entity.target)) entity.target = find_nearest_enemy(simulation, entity, DETECTION_RANGE);
    if (simulation->ent_alive(entity.target) && ai_state_is_passive(entity.ai_state)) entity.ai_state = kAggro;
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
        case kAggro: {
            if (!simulation->ent_alive(entity.target)) {
                entity.ai_state = kIdle;
                entity.ai_ticks_to_next_action = 0;
                entity.target = NULL_ENTITY;
                break;
            }
            Entity &target = simulation->get_ent(entity.target);
            Vector diff(target.x - entity.x, target.y - entity.y);
            float dist = diff.magnitude();
            entity.set_angle(diff.angle());
            if (dist > 750) {
                entity.ai_state = kIdle;
                entity.ai_ticks_to_next_action = 0;
                entity.target = NULL_ENTITY;
                break;
            }
            else if (dist > 250) entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION);
            if (entity.ai_ticks_to_next_action > SERVER_TIME(3) && dist < 500) {
                Entity &petal = simulation->alloc_petal(PetalId::kMissile);
                petal.set_x(entity.x);
                petal.set_y(entity.y);
                petal.set_angle(entity.angle);
                petal.set_parent(entity.id);
                petal.set_team(entity.team);
                petal.set_health(10);
                petal.set_max_health(10);
                petal.damage = 10;
                petal.velocity.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION * 4 / petal.friction);
                petal.detached = 1;
                petal.effect_delay = SERVER_TIME(3);
                entity.ai_ticks_to_next_action = 0;
                break;
            }
            break;
        }
        default:
            entity.ai_state = kIdle;
            break;
    }
}

void tick_centipede_ai(Simulation *simulation, Entity &entity) {
    switch(entity.ai_state) {
        case kIdle: {
            entity.ai_ticks_to_next_action = 0;
            entity.ai_state = frand() > 0.5 ? kIdleMoving : kIdleMoving2;
            break;
        }
        case kIdleMoving: {
            if (entity.ai_ticks_to_next_action > SERVER_TIME(5.0)) {
                entity.ai_ticks_to_next_action = 0;
                entity.ai_state = kIdle;
                break;
            }
            entity.set_angle(entity.angle + REAL_TIME(0.05));
            entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION / 10);
            break;
        }
        case kIdleMoving2: {
            if (entity.ai_ticks_to_next_action > SERVER_TIME(5.0)) {
                entity.ai_ticks_to_next_action = 0;
                entity.ai_state = kIdle;
                break;
            }
            entity.set_angle(entity.angle - REAL_TIME(0.05));
            entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION / 10);
            break;
        }
        default:
            entity.ai_state = kIdle;
            break;
    }
}

void tick_evil_centipede_ai(Simulation *simulation, Entity &entity) {
    if (!simulation->ent_alive(entity.target)) entity.target = find_nearest_enemy(simulation, entity, DETECTION_RANGE);
    if (simulation->ent_alive(entity.target) && ai_state_is_passive(entity.ai_state)) entity.ai_state = kAggro;
    switch(entity.ai_state) {
        case kIdle: {
            entity.ai_ticks_to_next_action = 0;
            entity.ai_state = frand() > 0.5 ? kIdleMoving : kIdleMoving2;
            break;
        }
        case kIdleMoving: {
            if (entity.ai_ticks_to_next_action > SERVER_TIME(5.0)) {
                entity.ai_ticks_to_next_action = 0;
                entity.ai_state = kIdle;
                break;
            }
            entity.set_angle(entity.angle + REAL_TIME(0.05));
            entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION / 10);
            break;
        }
        case kIdleMoving2: {
            if (entity.ai_ticks_to_next_action > SERVER_TIME(5.0)) {
                entity.ai_ticks_to_next_action = 0;
                entity.ai_state = kIdle;
                break;
            }
            entity.set_angle(entity.angle - REAL_TIME(0.05));
            entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION / 10);
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
            if (diff.magnitude() > DETECTION_RANGE + 300) {
                entity.ai_state = kIdle;
                entity.ai_ticks_to_next_action = 0;
                entity.target = NULL_ENTITY;
                break;
            }
            break;
        }
        default:
            entity.ai_state = kIdle;
            break;
    }
}

void tick_spider_ai(Simulation *simulation, Entity &entity) {
    if (!simulation->ent_alive(entity.target)) entity.target = find_nearest_enemy(simulation, entity, DETECTION_RANGE);
    if (simulation->ent_alive(entity.target) && ai_state_is_passive(entity.ai_state)) entity.ai_state = kAggro;
    switch(entity.ai_state) {
        case kIdle: {
            default_tick_idle(simulation, entity);
            break;
        }
        case kIdleMoving: {
            default_tick_idle_moving(simulation, entity);
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
            entity.acceleration.unit_normal(entity.angle).set_magnitude(PLAYER_ACCELERATION * 1.2);
            if (diff.magnitude() > DETECTION_RANGE + 300) {
                entity.ai_state = kIdle;
                entity.ai_ticks_to_next_action = 0;
                entity.target = NULL_ENTITY;
                break;
            }
            break;
        }
        default:
            entity.ai_state = kIdle;
            break;
    }
}

void tick_mob_ai(Simulation *simulation, Entity &entity) {
    if (entity.has_component(kSegmented) && simulation->ent_alive(entity.head_node)) return;
    if (simulation->ent_alive(entity.parent)) {
        Entity &parent = simulation->get_ent(entity.parent);
        Vector delta(parent.x - entity.x, parent.y - entity.y);
        float return_dist = 200;
        if (simulation->ent_alive(entity.target)) return_dist = DETECTION_RANGE + 300;
        else if (ai_state_is_passive(entity.ai_state)) return_dist = 350; 
        if (delta.magnitude() > return_dist) {
            entity.ai_state = kReturning;
            entity.ai_ticks_to_next_action = frand() * SERVER_TIME(0.25);
            entity.acceleration = delta.set_magnitude(PLAYER_ACCELERATION);
            entity.set_angle(delta.angle());
            entity.target = NULL_ENTITY;
            return;
        }
        /*
        else if (entity.ai_state == kReturning) {
            entity.acceleration = delta.set_magnitude(PLAYER_ACCELERATION / 2);
            entity.set_angle(delta.angle());
            entity.target = NULL_ENTITY;
            if (++entity.ai_ticks_to_next_action > SERVER_TIME(0.75)) {
                entity.ai_state = kIdle;
                entity.ai_ticks_to_next_action = SERVER_TIME(0.5) + frand() * SERVER_TIME(2);
            }
        }
        */
    }
    entity.ai_ticks_to_next_action++;// -= entity.ai_ticks_to_next_action > 0;
    switch(entity.mob_id) {
        case MobId::kBabyAnt:
        case MobId::kLadybug:
        case MobId::kMassiveLadybug:
            tick_baby_ant_ai(simulation, entity);
            break;
        case MobId::kWorkerAnt:
        case MobId::kDarkLadybug:
            tick_worker_ant_ai(simulation, entity);
            break;
        case MobId::kBeetle:
        case MobId::kSoldierAnt:
        case MobId::kMassiveBeetle:
            tick_soldier_ant_ai(simulation, entity);
            break;
        case MobId::kHornet:
            tick_hornet_ai(simulation, entity);
            break;
        case MobId::kBee:
            tick_bee_ai(simulation, entity);
            break;
        case MobId::kCentipede:
            tick_centipede_ai(simulation, entity);
            break;
        case MobId::kEvilCentipede:
            tick_evil_centipede_ai(simulation, entity);
            break;
        case MobId::kSpider:
            tick_spider_ai(simulation, entity);
            break;
        case MobId::kRock:
        case MobId::kBoulder:
        case MobId::kCactus:
            break;
        default:
            assert(!"invalid mobID");
            break;
    }
}