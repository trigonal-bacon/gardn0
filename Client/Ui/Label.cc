#include <Client/Ui/Label.hh>

#include <Client/Ui/ExternDef.hh>

using namespace ui;

LabelText::LabelText(char const *v, float f, uint32_t c) : text(v), font_size(f), color(c) {
    Renderer &ctx = *g_renderer;
    ctx.set_text_size(f);
    cache_size = ctx.get_text_size(v);
}

void LabelText::render(Renderer &ctx) {
    ctx.set_text_size(font_size);
    ctx.set_fill(color);
    ctx.set_stroke(0xff222222);
    ctx.set_line_width(font_size * 0.12);
    ctx.stroke_text(text);
    ctx.fill_text(text);
    //cache_size = ctx.get_text_size(text);
}

StaticLabel::StaticLabel(LabelText l) : text(std::move(l)) {
    width = l.cache_size;
    height = l.font_size;
}

void StaticLabel::on_render(Renderer &ctx) {
    ctx.center_text_align();
    ctx.center_text_baseline();
    text.render(ctx);
    //width = text.cache_size;
}

DynamicLabel::DynamicLabel(std::function<void(char *)> update_func, float s, uint32_t c) : text_update(std::move(update_func)), font_size(s), color(c) {
    height = s;
    animate = [&](Element *elt, Renderer &ctx){
        text_update(buf);
        float prev_w = width;
        ctx.set_text_size(font_size);
        width = ctx.get_text_size(buf);
        if (prev_w != width) invalidate_layout();
    };
}

void DynamicLabel::on_render(Renderer &ctx) {
    ctx.center_text_align();
    ctx.center_text_baseline();
    ctx.set_text_size(font_size);
    ctx.set_fill(color);
    ctx.set_stroke(0xff222222);
    ctx.set_line_width(font_size * 0.12);
    ctx.stroke_text(buf);
    ctx.fill_text(buf);
    //width = text.cache_size;
}