#include <Client/Render/RenderEntity.hh>

#include <Client/Render/Renderer.hh>

#include <Shared/Entity.hh>
#include <Shared/StaticData.hh>

void render_petal(Entity &ent, Renderer &ctx) {
    ctx.set_global_alpha(1 - ent.lerp_deletion_tick * 0.2);
    ctx.scale(1 + 0.1 * ent.lerp_deletion_tick);
    switch(ent.petal_id) {
        case kPetalId::kNone:
            break;
        case kPetalId::kBasic:
        case kPetalId::kFast:
        case kPetalId::kTwin:
            ctx.set_fill(0xffffffff);
            ctx.set_stroke(0xffcfcfcf);
            ctx.set_line_width(3);
            ctx.begin_path();
            ctx.arc(0,0,ent.radius);
            ctx.fill();
            ctx.stroke();
            break;
        case kPetalId::kStinger:
        case kPetalId::kTringer:
            ctx.set_fill(0xff333333);
            ctx.set_stroke(0xff292929);
            ctx.set_line_width(3);
            ctx.round_line_cap();
            ctx.round_line_join();
            ctx.begin_path();
            ctx.move_to(ent.radius,0);
            ctx.line_to(-ent.radius*0.5,ent.radius*0.866);
            ctx.line_to(-ent.radius*0.5,-ent.radius*0.866);
            ctx.line_to(ent.radius,0);
            ctx.fill();
            ctx.stroke();
            break;
        default:
            break;
    }
}