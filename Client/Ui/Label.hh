#pragma once 

#include <Client/Ui/Element.hh>

#include <initializer_list>
#include <string>
#include <vector>

namespace ui {
    class LabelText {
    public:
        char const *text;
        float font_size;
        uint32_t color;
        float cache_size;
        LabelText(char const *, float, uint32_t = 0xffffffff);

        void render(Renderer &);
    };

    class StaticLabel final : public Element {
    public:
        LabelText text;
        StaticLabel(LabelText);

        virtual void on_render(Renderer &) override;
    };
};