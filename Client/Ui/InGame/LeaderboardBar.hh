#pragma once

#include <Client/Ui/Element.hh>

namespace ui {
    class LeaderboardBar : public Element {
    public:
        float lerp_score;
        uint8_t pos;
        LeaderboardBar(uint8_t);

        virtual void on_render(Renderer &) override;
    };
}