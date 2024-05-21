#include <Client/Render/RenderEntity.hh>

#include <Client/Render/Renderer.hh>

#include <Client/Assets/Assets.hh>

#include <Shared/Entity.hh>
#include <Shared/StaticData.hh>

#include <Client/Game.hh>

#include <cmath>

void render_mob(Entity &ent, Renderer &ctx) {
    ctx.set_global_alpha(1 - ent.lerp_deletion_tick * 0.2);
    ctx.scale(1 + 0.1 * ent.lerp_deletion_tick);
    ctx.add_color_filter(0xffac0000, ent.damage_flash);
    draw_static_mob(ent.mob_id, ctx, ent.animation_tick, ent.radius, (ent.team == gardn->camera_id) | ((!ent.is_head) << 1) + (ent.id.id << 2));
    /*
    ctx.set_stroke(0xff000000);
    ctx.set_line_width(2);
    ctx.begin_path();
    ctx.arc(0,0,ent.radius);
    ctx.stroke();
    */
}