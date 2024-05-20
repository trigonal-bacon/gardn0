#pragma once

#include <Shared/Helpers.hh>

#include <Client/Ui/VisualUtils.hh>

#include <Client/Render/Renderer.hh>

#include <functional>

namespace ui {

    enum UiEvent {
        kMouseHover,
        kMouseDown,
        kMouseMove,
        kClick,
        kMouseUp,
        kFocusLost
    };

    class Layout {
    public:
        float width;
        float height;
        Layout();
        Layout(float, float);
    };

    class Element {
    public:
        VisualData visuals;
        float x = 0;
        float y = 0;
        float screen_x = 0;
        float screen_y = 0;
        float width = 0;
        float height = 0;
        float h_justify = 0;
        float v_justify = 0;
        uint8_t focused = 0;
        uint8_t rendering = 1; //default to show
        uint8_t not_first = 0;
        uint8_t detached = 0; //detached from container positioning process
        uint8_t layout_invalid = 1;
        LerpValue render_animation;
        Element *parent = nullptr;
        std::function<uint8_t(void)> should_render = [](){return 1;};
        std::function<void(Element *, Renderer &)> animate;
        Element();
        Element(float, float);

        Element *set_dimensions(float, float);
        void render(Renderer &);
        void refactor();
        void invalidate_layout();
        virtual void on_render(Renderer &);
        virtual void on_render_skip();
        virtual void on_refactor();
        virtual void on_poll_events();
        virtual Layout get_layout();

        void emit_event();
        virtual void on_event(uint8_t);
    };

    template<int hj, int vj>
    Element *Justify(Element *s) {
        s->h_justify = hj;
        s->v_justify = vj;
        return s;
    }
};