#pragma once

#include <Client/Ui/Element.hh>

#include <stdint.h>

namespace ui {
    class LevelBar : public Element {
    public:
        float cache_level_pct = 0;
        uint32_t level_on = 1;
        LevelBar();
        virtual void on_render(Renderer &) override;
    };
}