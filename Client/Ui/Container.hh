#pragma once 

#include <Client/Ui/Element.hh>

#include <initializer_list>
#include <vector>

namespace ui {
    class Container : public Element {
    public:
        std::vector<Element *> elements;
        float outer_pad = 0;
        float inner_pad = 0;
        Container(std::initializer_list<Element *>);

        void add_child(Element *);
        Element *pad(float, float);

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
        HContainer(std::initializer_list<Element *>);
        virtual void on_refactor() override;
    };

    class VContainer : public Container {
    public:
        VContainer(std::initializer_list<Element *>);
        virtual void on_refactor() override;
    };
}