#include <Client/Render/RenderEntity.hh>

#include <Client/Render/Renderer.hh>

#include <Shared/Entity.hh>

void render_flower(Entity &ent, Renderer &ctx) {
    ctx.set_stroke(0xffcfbb50);
    ctx.set_fill(0xffffe763);
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
}