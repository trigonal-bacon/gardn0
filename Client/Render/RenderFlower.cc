#include <Client/Render/RenderEntity.hh>

#include <Client/Render/Renderer.hh>

#include <Shared/Entity.hh>

void render_flower(Entity &ent, Renderer &ctx) {
    ctx.set_global_alpha(1 - ent.lerp_deletion_tick * 0.2);
    ctx.scale(1 + 0.1 * ent.lerp_deletion_tick);
    ctx.add_color_filter(0xffac0000, ent.damage_flash);
    uint32_t base_color = 0xffffe763;
    if (BIT_AT(ent.face_flags, 2)) base_color = 0xffce76db;
    ctx.set_stroke(Renderer::HSV(base_color, 0.8));
    ctx.set_fill(base_color);
    ctx.set_line_width(3);
    ctx.begin_path();
    ctx.arc(0, 0, ent.radius);
    ctx.fill();
    ctx.stroke();
    ctx.scale(ent.radius / 25);
    {
        RenderContext context(&ctx);
        ctx.set_fill(0xff222222);
        ctx.begin_path();
        ctx.ellipse(-7, -5, 3.25, 6.5);
        ctx.ellipse(7, -5, 3.25, 6.5);
        ctx.fill();
        ctx.begin_path();
        ctx.ellipse(-7, -5, 3, 6);
        ctx.ellipse(7, -5, 3, 6);
        ctx.clip();
        ctx.set_fill(0xffffffff);
        ctx.begin_path();
        ctx.arc(-7 + ent.lerp_eye_x, -5 + ent.lerp_eye_y, 3);
        ctx.arc(7 + ent.lerp_eye_x, -5 + ent.lerp_eye_y, 3);
        ctx.fill();
    }
    ctx.set_stroke(0xff222222);
    ctx.set_line_width(1.5);
    ctx.round_line_cap();
    ctx.begin_path();
    ctx.move_to(-6, 10);
    ctx.qcurve_to(0, ent.lerp_mouth, 6, 10);
    ctx.stroke();
    if (ent.lerp_mouth <= 8)
    {
        RenderContext context(&ctx);
        ctx.translate(0, -ent.lerp_mouth - 8);
        ctx.set_fill(base_color);
        ctx.begin_path();
        ctx.move_to(-12, 0);
        ctx.line_to(12, 0);
        ctx.line_to(0, 6);
        ctx.line_to(-12, 0);
        ctx.fill();
    }
}