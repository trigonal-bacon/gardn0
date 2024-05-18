#pragma once

#include <stdint.h>
#include <functional>

namespace ui {
    class Element;

    class VisualData {
    public:
        uint32_t fill;
        float stroke_darken;
        float line_width;
        float round_radius;
        VisualData();
        static VisualData ButtonBackground(uint32_t);
        static VisualData PanelBackground(uint32_t);
    };

    Element *AddTheme(VisualData, Element *);
    Element *SetRenderCondition(std::function<uint8_t(void)>, Element *);
};