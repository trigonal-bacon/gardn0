#include <Server/EntityFunctions.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>
#include <Shared/Helpers.hh>

#include <algorithm>
#include <iostream>

static void deal_poison(Entity &recv, Entity &deal) {
    float curr_pend_dmg = recv.applied_poison.damage * recv.applied_poison.ticks_left;
    float could_dmg = deal.poison.damage * deal.poison.ticks;
    if (curr_pend_dmg < could_dmg) {
        recv.applied_poison.dealer = deal.id;
        recv.applied_poison.damage = deal.poison.damage;
        recv.applied_poison.ticks_left = deal.poison.ticks;
    }
}

void inflict_damage(Simulation *simulation, Entity &receiver, EntityId &dealer, float damage) {
    assert(receiver.has_component(kHealth));
    //if (receiver.pending_delete) return;
    Entity &d_ent = simulation->get_ent(dealer);
    float rh = fclamp(receiver.health - damage, 0, receiver.max_health);
    receiver.set_health(rh);
    receiver.set_damaged(1);
    if (!simulation->ent_alive(receiver.target)) { 
        if (simulation->ent_alive(simulation->get_ent(dealer).parent)) receiver.target = simulation->get_ent(dealer).parent;
        else receiver.target = dealer; 
    }
    if (d_ent.poison.has()) deal_poison(receiver, d_ent);
    if (rh == 0) {
        simulation->request_delete(receiver.id);
        //dead
        if (!receiver.has_component(kScore)) return;
        if (d_ent.has_component(kFlower)) {
            d_ent.set_score(d_ent.score + receiver.score / 2);
            return;
        }
        if (simulation->ent_alive(d_ent.parent)) {
            if (!simulation->get_ent(d_ent.parent).has_component(kFlower)) return;
            Entity &d_ent_parent = simulation->get_ent(d_ent.parent);
            d_ent_parent.set_score(d_ent_parent.score + receiver.score / 2);
            return;
        }
        return;
    }
}

void inflict_heal(Simulation *simulation, Entity &receiver, float heal) {
    assert(receiver.has_component(kHealth));
    //if (receiver.pending_delete) return;
    float rh = fclamp(receiver.health + heal, 0, receiver.max_health);
    receiver.set_health(rh);
}

EntityId find_nearest_enemy(Simulation *simulation, Entity &entity, float radius) {
    EntityId ret;
    float min_dist = radius;
    simulation->spatial_hash.query(entity.x, entity.y, radius, radius, [&](Simulation *sim, Entity &ent){
        if (ent.team == entity.team) return;
        if (ent.immunity_ticks > 0) return;
        if (!ent.has_component(kMob) && !ent.has_component(kFlower)) return;
        float dist = Vector(ent.x-entity.x,ent.y-entity.y).magnitude();
        if (dist < min_dist) { min_dist = dist; ret = ent.id; }
    });
    return ret;
}