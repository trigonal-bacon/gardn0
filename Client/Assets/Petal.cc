#include <Client/Assets/Assets.hh>

#include <Shared/Helpers.hh>
#include <Shared/StaticData.hh>

#include <cmath>

void draw_static_petal_single(uint8_t id, Renderer &ctx) {
    switch(id) {
        case PetalId::kNone:
            break;
        case PetalId::kBasic:
        case PetalId::kFast:
        case PetalId::kTwin:
        case PetalId::kTriplet:
            ctx.set_fill(0xffffffff);
            ctx.set_stroke(0xffcfcfcf);
            ctx.set_line_width(3);
            ctx.begin_path();
            ctx.arc(0,0,PETAL_DATA[id].radius);
            ctx.fill();
            ctx.stroke();
            break;
        case PetalId::kStinger:
        case PetalId::kTringer:
            ctx.set_fill(0xff333333);
            ctx.set_stroke(0xff292929);
            ctx.set_line_width(3);
            ctx.round_line_cap();
            ctx.round_line_join();
            ctx.begin_path();
            ctx.move_to(PETAL_DATA[id].radius,0);
            ctx.line_to(-PETAL_DATA[id].radius*0.5,PETAL_DATA[id].radius*0.866);
            ctx.line_to(-PETAL_DATA[id].radius*0.5,-PETAL_DATA[id].radius*0.866);
            ctx.line_to(PETAL_DATA[id].radius,0);
            ctx.fill();
            ctx.stroke();
            break;
        case PetalId::kLeaf:
            ctx.set_fill(0xff39b54a);
            ctx.set_stroke(0xff2e933c);
            ctx.set_line_width(3);
            ctx.round_line_cap();
            ctx.round_line_join();
            ctx.begin_path();
            ctx.move_to(-20, 0);
            ctx.line_to(-15, 0);
            ctx.bcurve_to(-10,-12,5,-12,15,0);
            ctx.bcurve_to(5,12,-10,12,-15,0);
            ctx.fill();
            ctx.stroke();
            ctx.begin_path();
            ctx.move_to(-9,0);
            ctx.qcurve_to(0,-1.5,7.5,0);
            ctx.stroke();
            break;
        case PetalId::kWing:
            ctx.begin_path();
            ctx.partial_arc(0,0,15,-1.5707963267948966,1.5707963267948966,0);
            ctx.qcurve_to(10,0,0,-15);
            ctx.set_fill(0xffffffff);
            ctx.fill();
            ctx.set_stroke(0xffcfcfcf);
            ctx.set_line_width(3);
            ctx.round_line_cap();
            ctx.round_line_join();
            ctx.stroke();
            break;
        case PetalId::kEgg:
            ctx.set_fill(0xffcfc295);
            ctx.begin_path();
            ctx.arc(0,0,PETAL_DATA[id].radius);
            ctx.fill();
            ctx.set_fill(0xfffff0b8);
            ctx.begin_path();
            ctx.arc(0,0,PETAL_DATA[id].radius * 0.8);
            ctx.fill();
            break;
        case PetalId::kBeetleEgg:
            ctx.begin_path();
            ctx.ellipse(0,0,PETAL_DATA[id].radius * 0.85,PETAL_DATA[id].radius * 1.15);
            ctx.set_fill(0xfffff0b8);
            ctx.fill();
            ctx.set_stroke(0xffcfc295);
            ctx.set_line_width(3);
            ctx.stroke();
            break;
        case PetalId::kIris:
        case PetalId::kGrapes:
            ctx.set_fill(0xffce76db);
            ctx.set_stroke(0xffa760b1);
            ctx.set_line_width(3);
            ctx.begin_path();
            ctx.arc(0,0,PETAL_DATA[id].radius);
            ctx.fill();
            ctx.stroke();
            break;
        case PetalId::kMissile:
            ctx.set_fill(0xff222222);
            ctx.set_stroke(0xff222222);
            ctx.set_line_width(5.0);
            ctx.round_line_cap();
            ctx.round_line_join();
            ctx.begin_path();
            ctx.move_to(11.0, 0.0);
            ctx.line_to(-11.0, -6.0);
            ctx.line_to(-11.0, 6.0);
            ctx.line_to(11.0, 0.0);
            ctx.fill();
            ctx.stroke();
            break;
        case PetalId::kBubble:
            ctx.begin_path();
            ctx.arc(0,0,12);
            ctx.set_stroke(0xb2ffffff);
            ctx.set_line_width(3);
            ctx.stroke();
            ctx.begin_path();
            ctx.arc(0,0,10.5);
            ctx.set_fill(0x59ffffff);
            ctx.fill();
            ctx.begin_path();
            ctx.arc(4,-4,3);
            ctx.set_fill(0x59ffffff);
            ctx.fill();
            break;
        case PetalId::kPeas:
            ctx.set_fill(0xff8ac255);
            ctx.set_stroke(0xff709d45);
            ctx.set_line_width(3);
            ctx.begin_path();
            ctx.arc(0,0,PETAL_DATA[id].radius);
            ctx.fill();
            ctx.stroke();
            break;
        case PetalId::kRock: {
            SeedGenerator gen(PETAL_DATA[id].radius * 743 + 294);
            ctx.set_fill(0xff777777);
            ctx.set_stroke(Renderer::HSV(0xff777777, 0.8));
            ctx.set_line_width(3);
            ctx.round_line_cap();
            ctx.round_line_join();
            ctx.begin_path();
            ctx.move_to(PETAL_DATA[id].radius + (gen.next() - 0.5) * 2,(gen.next() - 0.5) * 2);
            for (uint32_t i = 1; i < 5; ++i) {
                float angle = 2 * M_PI * i / 5.0f;
                ctx.line_to(cosf(angle) * PETAL_DATA[id].radius + (gen.next() - 0.5) * 2, sinf(angle) * PETAL_DATA[id].radius + (gen.next() - 0.5) * 2);
            }
            ctx.close_path();
            ctx.fill();
            ctx.stroke();
            break;
        }
        case PetalId::kHeavy:
            ctx.set_fill(0xffaaaaaa);
            ctx.set_stroke(0xff888888);
            ctx.set_line_width(3);
            ctx.begin_path();
            ctx.arc(0,0,PETAL_DATA[id].radius);
            ctx.fill();
            ctx.stroke();
            break;
        case PetalId::kHeaviest:
            ctx.begin_path();
            ctx.arc(0,0,16);
            ctx.set_fill(0xff333333);
            ctx.fill();
            ctx.set_stroke(0xff292929);
            ctx.set_line_width(3);
            ctx.stroke();
            ctx.begin_path();
            ctx.arc(6,-6,4.6);
            ctx.set_fill(0xffcccccc);
            ctx.fill();
            break;
        default:
            break;
    }
}

void draw_static_petal(uint8_t id, Renderer &ctx) {
    for (uint32_t i = 0; i < PETAL_DATA[id].count; ++i) {
        RenderContext context(&ctx);
        float rad = PETAL_DATA[id].clump_radius;
        rad += (rad == 0) * 10;
        ctx.rotate(i * 2 * M_PI / PETAL_DATA[id].count);
        if (PETAL_DATA[id].count > 1) ctx.translate(rad, 0);
        if (id == PetalId::kLeaf) ctx.rotate(-1);
        if (id == PetalId::kWing || id == PetalId::kMissile) ctx.rotate(1);
        draw_static_petal_single(id, ctx);
    }
}
