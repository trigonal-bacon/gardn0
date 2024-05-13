#include <Server/EntityFunctions.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>
#include <Shared/Helpers.hh>

void inflict_damage(Simulation *simulation, Entity &receiver, EntityId &dealer, float damage) {
    assert(receiver.has_component(kHealth));
    //if (receiver.pending_delete) return;
    float rh = fclamp(receiver.health - damage, 0, receiver.max_health);
    receiver.set_health(rh);
    receiver.set_damaged(1);
    if (rh == 0) {
        simulation->request_delete(receiver.id);
        //dead
        if (!receiver.has_component(kScore)) return;
        Entity &d_ent = simulation->get_ent(dealer);
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
    if (!simulation->ent_alive(receiver.target)) receiver.target = dealer.id;
}

void inflict_heal(Simulation *simulation, Entity &receiver, float heal) {
    assert(receiver.has_component(kHealth));
    //if (receiver.pending_delete) return;
    float rh = fclamp(receiver.health + heal, 0, receiver.max_health);
    receiver.set_health(rh);
}
