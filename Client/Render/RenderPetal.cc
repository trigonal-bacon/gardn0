#include <Client/Render/RenderEntity.hh>

#include <Client/Render/Renderer.hh>

#include <Client/Assets/Assets.hh>

#include <Shared/Entity.hh>
#include <Shared/StaticData.hh>

void render_petal(Entity &ent, Renderer &ctx) {
    ctx.set_global_alpha(1 - ent.lerp_deletion_tick * 0.2);
    ctx.scale(1 + 0.1 * ent.lerp_deletion_tick);
    ctx.add_color_filter(0xffac0000, ent.damage_flash);
    draw_static_petal_single(ent.petal_id, ctx);
}