#include <Client/Render/RenderEntity.hh>

#include <Client/Render/Renderer.hh>

#include <Shared/Entity.hh>
#include <Shared/StaticData.hh>

void render_mob(Entity &ent, Renderer &ctx) {
    ctx.set_global_alpha(1 - ent.lerp_deletion_tick * 0.2);
    ctx.scale(1 + 0.1 * ent.lerp_deletion_tick);
    switch(ent.mob_id) {
        case kMobId::kBabyAnt:
            ctx.set_stroke(0xff292929);
            ctx.set_line_width(7);
            ctx.round_line_cap();
            ctx.begin_path();
            ctx.move_to(0, -7);
            ctx.qcurve_to(11, -10, 22, -5);
            ctx.move_to(0, 7);
            ctx.qcurve_to(11, 10, 22, 5);
            ctx.stroke();
            ctx.set_fill(0xff555555);
            ctx.set_stroke(0xff454545);
            ctx.begin_path();
            ctx.arc(0,0,ent.radius);
            ctx.fill();
            ctx.stroke();
            break;
    }
}