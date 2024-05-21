#include <Client/Ui/VisualUtils.hh>

#include <Client/Ui/Element.hh>

using namespace ui;

VisualData::VisualData() : fill(0x00000000), stroke_darken(0.8), line_width(6), round_radius(2) {}

VisualData VisualData::ButtonBackground(uint32_t color) {
    VisualData v;
    v.fill = color;
    v.line_width = 3;
    return v;
}

VisualData VisualData::PanelBackground(uint32_t color) {
    VisualData v;
    v.fill = color;
    v.line_width = 6;
    v.round_radius = 2;
    return v;
}

Element *ui::AddTheme(VisualData v, Element *t) {
    t->visuals = v;
    return t;
}

Element *ui::SetRenderCondition(std::function<uint8_t(void)> f, Element *t) {
    t->should_render = f;
    return t;
}
