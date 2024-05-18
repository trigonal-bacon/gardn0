#include <Client/Ui/Button.hh>

#include <Client/Ui/ExternDef.hh>

using namespace ui;

Button::Button(float w, float h, Element *inner, std::function<void(void)> f) : Element(w,h), label(inner), onclick(std::move(f)) {
    inner->h_justify = inner->v_justify = 0;
    inner->x = inner->y = 0;
}

void Button::on_render(Renderer &ctx) {
    ctx.add_color_filter(0xffffffff, 0.1 * focused);
    ctx.set_fill(visuals.fill);
    ctx.set_stroke(Renderer::HSV(visuals.fill, visuals.stroke_darken));
    ctx.set_line_width(visuals.line_width);
    ctx.begin_path();
    ctx.round_rect(-width / 2, -height / 2, width, height, visuals.round_radius);
    ctx.fill();
    ctx.stroke();
    label->render(ctx);
}

void Button::on_event(uint8_t type) {
    if (type == kClick) onclick();
}