#include <Client/Ui/InGame/LeaderboardBar.hh>

#include <Client/Game.hh>

#include <algorithm>
#include <string>

using namespace ui;

LeaderboardBar::LeaderboardBar(uint8_t p) : pos(p), lerp_score(0), Element(180, 20) {
    should_render = [&](){
        return pos < gardn->simulation.leaderboard.length;
    };
    animate = [&](Element *self, Renderer &ctx){
        float _d = 1 - (pow(1 - 0.2, g_frame_dt * 60 / 1000));
        LERP(lerp_score, gardn->simulation.leaderboard[pos].score, _d);
        ctx.scale(self->render_animation);
    };
}

void LeaderboardBar::on_render(Renderer &ctx) {
    ctx.set_stroke(0xff222222);
    ctx.set_line_width(height);
    ctx.round_line_cap();
    ctx.begin_path();
    ctx.move_to(-width / 2, 0);
    ctx.line_to(width / 2, 0);
    ctx.stroke();
    float ratio = fclamp(lerp_score / gardn->simulation.leaderboard[0].score, 0, 1);
    if (gardn->simulation.leaderboard[0].score == 0) ratio = 1;
    if (gardn->simulation.get_ent(gardn->camera_id).player == gardn->simulation.leaderboard[pos].id) ctx.set_stroke(0xffa2a222);
    else ctx.set_stroke(0xff22a222);
    ctx.set_line_width(height - 4);
    ctx.round_line_cap();
    ctx.begin_path();
    ctx.move_to(-width / 2, 0);
    ctx.line_to(-width / 2 + width * ratio, 0);
    ctx.stroke();
    char buf[28];
    snprintf(buf, 27, "%s - %.0f", gardn->simulation.leaderboard[pos].name.c_str(), gardn->simulation.leaderboard[pos].score);
    ctx.set_fill(0xffffffff);
    ctx.set_stroke(0xff222222);
    ctx.set_text_size(14);
    ctx.set_line_width(14 * 0.12);
    ctx.center_text_align();
    ctx.center_text_baseline();
    ctx.stroke_text(buf);
    ctx.fill_text(buf);
}