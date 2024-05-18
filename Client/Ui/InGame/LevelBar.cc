#include <Client/Ui/InGame/LevelBar.hh>

#include <Client/Game.hh>

#include <stdio.h>

using namespace ui;

LevelBar::LevelBar() : Element (300,40) {
    y = -80;
}

void LevelBar::on_render(Renderer &ctx) {
    ctx.set_stroke(0xff222222);
    ctx.set_line_width(height);
    ctx.round_line_cap();
    ctx.begin_path();
    ctx.move_to(-width / 2, 0);
    ctx.line_to(width / 2, 0);
    ctx.stroke();
    float xp = gardn->simulation.get_ent(gardn->camera_id).experience;
    uint32_t lvl = 1; 
    while (xp > xp_to_next_level(lvl)) {
        xp -= xp_to_next_level(lvl);
        ++lvl;
    }
    //get_level_from_xp(xp);
    float _d = 1 - (pow(1 - 0.15, g_frame_dt * 60 / 1000));
    if (lvl == level_on) {
        LERP(cache_level_pct, xp / xp_to_next_level(lvl), _d);
    } else {
        LERP(cache_level_pct, 1, _d);
        if (cache_level_pct > 0.999) {
            level_on = lvl;
            cache_level_pct = 0;
        }
    }
    ctx.set_stroke(0xff75dd34);
    ctx.set_line_width(height * 0.8);
    ctx.begin_path();
    ctx.move_to(-width / 2, 0);
    ctx.line_to(-width / 2 + width * cache_level_pct, 0);
    ctx.stroke();
    char a[32];
    sprintf(a, "Level %u", level_on);
    ctx.set_fill(0xffffffff);
    ctx.set_stroke(0xff222222);
    ctx.set_text_size(height * 0.5);
    ctx.set_line_width(height * 0.5 * 0.12);
    ctx.center_text_align();
    ctx.center_text_baseline();
    ctx.stroke_text(a);
    ctx.fill_text(a);
    if (level_on < 45) {
        lvl = (level_on + 15) / 15 * 15;
        sprintf(a, "Extra slot at level %u", lvl);
        ctx.set_fill(0xffffffff);
        ctx.set_stroke(0xff222222);
        ctx.set_text_size(height * 0.4);
        ctx.set_line_width(height * 0.4 * 0.12);
        ctx.translate(0, height * 0.8);
        ctx.center_text_align();
        ctx.center_text_baseline();
        ctx.stroke_text(a);
        ctx.fill_text(a);
    }
}