#include <Client/Ui/InGame/LevelBar.hh>

#include <Client/Game.hh>

#include <Client/Assets/Assets.hh>

using namespace ui;

DeathFlower::DeathFlower() : Element(100, 100) {}

void DeathFlower::on_render(Renderer &ctx) {
    ctx.rotate(-0.25);
    float radius = 40;
    ctx.scale(radius / 25);
    {
        RenderContext context(&ctx);
        ctx.translate(25,-10);
        draw_static_petal(gardn->cache_loadout[4], ctx);
    }
    {
        RenderContext context(&ctx);
        ctx.translate(-2,-22);
        draw_static_petal(gardn->cache_loadout[3], ctx);
    }
    {
        RenderContext context(&ctx);
        ctx.translate(-28,-4);
        draw_static_petal(gardn->cache_loadout[2], ctx);
    }
    uint32_t base_color = 0xffffe763;
    ctx.set_stroke(Renderer::HSV(base_color, 0.8));
    ctx.set_fill(base_color);
    ctx.set_line_width(3);
    ctx.begin_path();
    ctx.arc(0, 0, 25);
    ctx.fill();
    ctx.stroke();
    {
        ctx.round_line_cap();
        ctx.set_stroke(0xff222222);
        ctx.set_line_width(2.5);
        ctx.begin_path();
        ctx.move_to(-10, -8);
        ctx.line_to(-4, -2);
        ctx.move_to(-4, -8);
        ctx.line_to(-10, -2);
        ctx.move_to(10, -8);
        ctx.line_to(4, -2);
        ctx.move_to(4, -8);
        ctx.line_to(10, -2);
        ctx.stroke();
    }
    ctx.set_stroke(0xff222222);
    ctx.set_line_width(1.5);
    ctx.round_line_cap();
    ctx.begin_path();
    ctx.move_to(-6, 10);
    ctx.qcurve_to(0, 5, 6, 10);
    ctx.stroke();
    {
        RenderContext context(&ctx);
        ctx.translate(-11,20);
        draw_static_petal(gardn->cache_loadout[1], ctx);
    }
    {
        RenderContext context(&ctx);
        ctx.translate(20,18);
        draw_static_petal(gardn->cache_loadout[0], ctx);
    }
}