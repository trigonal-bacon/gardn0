#include <Client/Ui/TitleScreen/Loadout.hh>

#include <Client/Game.hh>
#include <Client/Assets/Assets.hh>

using namespace ui;

TitleScreenLoadout::TitleScreenLoadout(uint8_t p) : Element(60,60), pos(p) {
    should_render = [&](){
        if (!gardn->simulation_ready) return false;
        //assume camera exists (it has to)
        render_id =gardn->simulation.get_ent(gardn->camera_id).loadout_ids[pos];
        return render_id != kNone && render_id != kBasic;
    };
    pad_x = pad_y = 4;
}

void TitleScreenLoadout::on_render(Renderer &ctx) {
    uint32_t color = RARITY_COLORS[PETAL_DATA[render_id].rarity];
    ctx.set_fill(Renderer::HSV(color, 0.8));
    ctx.scale(width / 60);
    ctx.begin_path();
    ctx.round_rect(-30, -30, 60, 60, 6);
    ctx.fill();
    ctx.set_fill(Renderer::HSV(color, 1.0));
    ctx.begin_path();
    ctx.round_rect(-25, -25, 50, 50, 2);
    ctx.fill();
    ctx.translate(0, -5);
    draw_static_petal(render_id, ctx);
    ctx.translate(0, 20);
    ctx.set_text_size(12);
    ctx.set_line_width(12 * 0.12);
    ctx.set_fill(0xffffffff);
    ctx.set_stroke(0xff222222);
    ctx.center_text_align();
    ctx.center_text_baseline();
    ctx.stroke_text(PETAL_DATA[render_id].name);
    ctx.fill_text(PETAL_DATA[render_id].name);
}