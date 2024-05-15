#include <Client/Ui/Element.hh>

namespace ui {
    class StaticSpace final : public Element {
    public:
        StaticSpace(float w, float h) : Element(w,h) {}
    };

    class Nothing final : public Element {
    public:
        Nothing() : Element() {}
    };
}