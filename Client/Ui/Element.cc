#include <Client/Ui/Ui.hh>

#include <cmath>
#include <iostream>

using namespace ui;

Layout::Layout() : width(0), height(0) {
}

Layout::Layout(float w, float h) : width(w), height(h) {
}

Element::Element() : animate([](Element *self, Renderer &ctx){ ctx.scale(self->render_animation); }) {}

Element::Element(float w, float h) : width(w) , height(h), animate([](Element *self, Renderer &ctx){ ctx.scale(self->render_animation); }) {}

void Element::render(Renderer &ctx) {
    screen_x = ctx.context.transform_matrix[2];
    screen_y = ctx.context.transform_matrix[5];
    render_animation = should_render();
    uint8_t prev_rendering = rendering;
    if (!not_first) render_animation.lerp_step(not_first);
    else render_animation.lerp_step(1 - (pow(1 - 0.35, g_frame_dt * 60 / 1000))); //fix
    rendering = render_animation > 0.01;
    if ((!not_first || rendering != prev_rendering) || layout_invalid) invalidate_layout();
    not_first = 1;
    #ifdef DEBUG
    if (0 && rendering) {
        RenderContext context(&ctx);
        ctx.set_stroke(0x80000000);
        ctx.set_line_width(1);
        ctx.begin_path();
        ctx.round_rect(-width / 2, -height / 2, width, height, 6);
        ctx.stroke();
    }
    #endif
    if (rendering) {
        animate(this, ctx);
        on_render(ctx);
    }
    else on_render_skip();
}

void Element::on_render(Renderer &ctx) {}

void Element::on_render_skip() {}

void Element::refactor() {
    if (!layout_invalid) return;
    layout_invalid = 0;
    on_refactor();
}

void Element::on_refactor() {}

void Element::invalidate_layout() {
    layout_invalid = 1;
    Element *par = parent;
    while (par != nullptr) {
        par->layout_invalid = 1;
        par = par->parent;
    }
}

Layout Element::get_layout() { return Layout(width, height); }

void Element::emit_event() {
    if (g_focused != this) {
        focused = 0;
        return on_event(kFocusLost);
    }
    focused = 1;
    if (BIT_AT(g_input->mouse_buttons_pressed, 0)) {
        return on_event(kMouseDown);
    } 
    if (BIT_AT(g_input->mouse_buttons_released, 0)) {
        if (focused) on_event(kClick);
        else on_event(kMouseUp);
        return;
    }
    if (BIT_AT(g_input->mouse_buttons_state, 0)) return on_event(kMouseMove);
    return on_event(kMouseHover);
}

void Element::on_event(uint8_t type) {}

void Element::on_poll_events() { 
    if (fabsf(g_input->mouse_x - screen_x) < width * g_scale / 2
    && fabsf(g_input->mouse_y - screen_y) < height * g_scale / 2) {
        g_focused = this;
    }
    else if (g_focused == this) {
        g_focused = g_window;
    }
}