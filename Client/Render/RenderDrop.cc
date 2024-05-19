#include <Client/Render/RenderEntity.hh>

#include <Client/Render/Renderer.hh>

#include <Client/Assets/Assets.hh>

#include <Shared/Entity.hh>
#include <Shared/StaticData.hh>

#include <cmath>

void render_drop(Entity &ent, Renderer &ctx) {
    float animation_value = sinf(ent.animation_tick);
    ctx.set_global_alpha(1 - ent.lerp_deletion_tick * 0.2);
    ctx.scale(1 - 0.2 * ent.lerp_deletion_tick);
    ctx.scale(1 + animation_value * 0.03);
    ctx.scale(ent.lerp_radius / 30);
    uint8_t rarity = PETAL_DATA[ent.drop_id].rarity;
    ctx.set_fill(0x80000000);
    ctx.begin_path();
    ctx.rect(-32, -32, 64, 64);
    ctx.fill();
    DRAW_LOADOUT_BG(RARITY_COLORS[rarity]);
    ctx.translate(0, -5);
    draw_static_petal(ent.drop_id, ctx);
    ctx.set_text_size(14);
    ctx.set_line_width(14 * 0.12);
    ctx.set_fill(0xffffffff);
    ctx.set_stroke(0xff222222);
    ctx.translate(0, 20);
    ctx.center_text_align();
    ctx.center_text_baseline();
    ctx.stroke_text(PETAL_DATA[ent.drop_id].name);
    ctx.fill_text(PETAL_DATA[ent.drop_id].name);
}