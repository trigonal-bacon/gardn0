#include <Client/Ui/Choose.hh>

using namespace ui;

Choose::Choose(Element *a, Element *b, std::function<uint8_t()> c) : A(a), B(b), chooser(c), rendering_B(0) {
    a->parent = b->parent = this;
    a->should_render = [&](){
        return rendering_B == 0;
    };
    b->should_render = [&](){
        return rendering_B != 0;
    };
    animate = [&](Element *self, Renderer &ctx){
        rendering_B = chooser();
    };
}

void Choose::on_refactor() {
    A->refactor();
    B->refactor();
    Element *use = rendering_B ? B : A;
    width = use->width;
    height = use->height;
}

void Choose::on_render(Renderer &ctx) {
    if (rendering_B) {
        B->render(ctx);
        A->on_render_skip();
    }
    else {
        A->render(ctx);
        B->on_render_skip();
    }
}