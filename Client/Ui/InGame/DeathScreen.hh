#pragma once

#include <Client/Ui/Element.hh>

namespace ui {
    class DeathFlower : public Element {
    public:
        DeathFlower();
        virtual void on_render(Renderer &) override;
    };
}