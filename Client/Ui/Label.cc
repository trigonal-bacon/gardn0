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
}

StaticLabel::StaticLabel(LabelText l) : text(std::move(l)) {
    width = l.cache_size;
    height = l.font_size;
}

void StaticLabel::on_render(Renderer &ctx) {
    ctx.center_text_align();
    ctx.center_text_baseline();
    text.render(ctx);
}