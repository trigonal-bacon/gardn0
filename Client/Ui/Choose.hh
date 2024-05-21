#include <Client/Ui/Element.hh>

#include <functional>
namespace ui {
    class Choose final : public Element {
    public:
        uint8_t rendering_B;
        Element *A;
        Element *B;
        std::function<uint8_t()> chooser;
        Choose(Element *, Element *, std::function<uint8_t()>);

        virtual void on_refactor() override;
        virtual void on_render(Renderer &) override;
    };
}