#pragma once 

#include <Client/Ui/Element.hh>

#include <initializer_list>
#include <vector>

namespace ui {
    class Container : public Element {
    public:
        std::vector<Element *> elements;
        Container(std::initializer_list<Element *>);

        void add_child(Element *);

        virtual void on_render(Renderer &) override;

        virtual void on_poll_events() override;
    };

    class Window final : public Container {
    public:
        Window();
        virtual void on_refactor() override;
        virtual void on_render(Renderer &) override;

        void render(Renderer &);
    };

    class HContainer : public Container {
    public:
        float outer_pad = 0;
        HContainer(std::initializer_list<Element *>);
        virtual void on_refactor() override;
    };

    class VContainer : public Container {
    public:
        float outer_pad = 0;
        VContainer(std::initializer_list<Element *>);
        virtual void on_refactor() override;
    };
}