#pragma once 

#include <Client/Ui/Element.hh>

#include <functional>

namespace ui {
    class Button : public Element {
    public:
        Element *label;
        std::function<void(void)> onclick;
        Button(float, float, Element *, std::function<void(void)>);

        virtual void on_render(Renderer &) override;
        virtual void on_event(uint8_t) override;
    };
}
