#include <Client/Assets/Assets.hh>

#include <Shared/Helpers.hh>
#include <Shared/StaticData.hh>

void draw_static_mob(uint8_t mob_id, Renderer &ctx, float animation_value) {
    switch(mob_id) {
        case kMobId::kBabyAnt:
            ctx.set_stroke(0xff292929);
            ctx.set_line_width(7);
            ctx.round_line_cap();
            ctx.begin_path();
            ctx.move_to(0, -7);
            ctx.qcurve_to(11, -10 + animation_value, 22, -5 + animation_value);
            ctx.move_to(0, 7);
            ctx.qcurve_to(11, 10 - animation_value, 22, 5 - animation_value);
            ctx.stroke();
            ctx.set_fill(0xff555555);
            ctx.set_stroke(0xff454545);
            ctx.begin_path();
            ctx.arc(0,0,MOB_DATA[mob_id].radius);
            ctx.fill();
            ctx.stroke();
            break;
    }
}