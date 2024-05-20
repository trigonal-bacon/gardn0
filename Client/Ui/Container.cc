#include <Client/Ui/Container.hh>

#include <Client/Ui/ExternDef.hh>

#include <iostream>

using namespace ui;

Container::Container(std::initializer_list<Element *> elts) : Element() {
    for (Element *elt : elts) { add_child(elt); std::cout << "hi" << elt << '\n'; }
}

void Container::add_child(Element *elt) {
    elt->parent = this;
    elements.push_back(elt);
}

void Container::on_render(Renderer &ctx) {
    for (Element *elt : elements) {
        RenderContext context(&ctx);
        ctx.translate(elt->x, elt->y);
        ctx.translate(elt->h_justify * (width - elt->width) / 2, elt->v_justify * (height - elt->height) / 2);
        elt->render(ctx);
    }
}

void Container::on_poll_events() {
    Element::on_poll_events();
    if (g_focused != this) return;
    for (Element *elt : elements) {
        if (!elt->should_render()) continue;
        elt->on_poll_events();
    }
}

Element *Container::pad(float o, float i) {
    outer_pad = o;
    inner_pad = i;
    return this;
}

Window::Window() : Container({}) {}

void Window::on_refactor() {
    for (Element *elt : elements) elt->refactor();
}

void Window::render(Renderer &ctx) {
    width = ctx.width; height = ctx.height;
    ctx.translate(width / 2, height / 2);
    screen_x = ctx.context.transform_matrix[2];
    screen_y = ctx.context.transform_matrix[5];
    on_render(ctx);
}

void Window::on_render(Renderer &ctx) {
    //x = width / 2; y = height / 2;
    //ctx.translate(x, y);
    for (Element *elt : elements) {
        RenderContext context(&ctx);
        //ctx.translate(elt->h_justify * (width - elt->width) / 2, elt->v_justify * (height - elt->height) / 2);
        ctx.translate(elt->h_justify * width / 2, elt->v_justify * height / 2);
        ctx.scale(g_scale);
        ctx.translate(-elt->h_justify * elt->width / 2, -elt->v_justify * elt->height / 2);
        ctx.translate(elt->x, elt->y);
        elt->render(ctx);
    }
}

HContainer::HContainer(std::initializer_list<Element *> elts) : Container(elts) {}

void HContainer::on_refactor() {
    float x = outer_pad;
    float y = 0;
    for (Element *elt : elements) {
        elt->h_justify = -1;
        elt->refactor();
        if (!elt->rendering || elt->detached) continue;
        Layout l = elt->get_layout();
        elt->x = x;
        elt->y = -elt->v_justify * outer_pad;
        x += l.width + inner_pad;
        y = fmax(y, l.height);
    }
    x += outer_pad - inner_pad;
    y += 2 * outer_pad;
    width = x;
    height = y;
}

VContainer::VContainer(std::initializer_list<Element *> elts) : Container(elts) {}

void VContainer::on_refactor() {
    float x = 0;
    float y = outer_pad;
    for (Element *elt : elements) {
        elt->v_justify = -1;
        elt->refactor();
        if (!elt->rendering || elt->detached) continue;
        Layout l = elt->get_layout();
        elt->x = -elt->h_justify * outer_pad;
        elt->y = y;
        x = fmax(x, l.width);
        y += l.height + inner_pad;
    }
    x += 2 * outer_pad;
    y += outer_pad - inner_pad;
    if (!immutable_length) width = x;
    height = y;
}

Element *VContainer::lock_length() {
    immutable_length = 1;
    return this;
} 