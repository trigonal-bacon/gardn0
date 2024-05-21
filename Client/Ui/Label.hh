#pragma once 

#include <Client/Ui/Element.hh>

#include <functional>
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

    class DynamicLabel final : public Element {
    public:
        char buf[64] = {0};
        float font_size;
        uint32_t color;
        std::function<void(char *)> text_update;
        DynamicLabel(std::function<void(char *)>, float, uint32_t = 0xffffffff);

        virtual void on_render(Renderer &) override;
    };
};