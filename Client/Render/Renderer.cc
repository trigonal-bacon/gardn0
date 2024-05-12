#include <Client/Render/Renderer.hh>

#include <Shared/Helpers.hh>

#include <cmath>
#include <iostream>
#include <emscripten.h>

RenderContext::RenderContext(Renderer *r) : renderer(r) {
    *this = r->context;
EM_ASM({
    Module.ctxs[$0].save();
}, r->id);
}

RenderContext::~RenderContext() {
    renderer->context = *this;
EM_ASM({
    Module.ctxs[$0].restore();
}, renderer->id);
}

Renderer::Renderer() : context(this) {
    id = EM_ASM_INT({
        if (Module.availableCtxs.length)
        {
            const index = Module.availableCtxs.shift();
            if (index == 0)
                return 0; // used for the main ctx, because that has special behavior
            const ocanvas = new OffscreenCanvas(1, 1);
            Module.ctxs[index] = ocanvas.getContext('2d');
            return index;
        }
        throw new Error(
            'Out of Contexts: Can be fixed by allowing more contexts');
        return -1;
    });
    DEBUG_ONLY(std::cout << "created canvas " << id << '\n';)
}

Renderer::~Renderer() {
    EM_ASM({
        if ($0 == 0)
            throw new Error('Tried to delete the main context');
        Module.ctxs[$0] = null;
        Module.availableCtxs.push($0);
    }, id);
    DEBUG_ONLY(std::cout << "removed canvas " << id << '\n';)
}

void Renderer::set_fill(uint32_t v) {
    EM_ASM({
    Module.ctxs[$0].fillStyle = "rgba("+$3+","+$2+","+$1+","+$4/255+")";
    }, id, v & 255, (v >> 8) & 255, (v >> 16) & 255, v >> 24);
}

void Renderer::set_stroke(uint32_t v) {
    EM_ASM({
    Module.ctxs[$0].strokeStyle = "rgba("+$3+","+$2+","+$1+","+$4/255+")";
    }, id, v & 255, (v >> 8) & 255, (v >> 16) & 255, v >> 24);
}

void Renderer::set_line_width(float v) {
    EM_ASM({
    Module.ctxs[$0].lineWidth = $1;
    }, id, v);
}

void Renderer::set_text_size(float v) {
    EM_ASM({
    Module.ctxs[$0].lineWidth = $1 + "px Ubuntu";
    }, id, v);
}

void Renderer::set_global_alpha(float v) {
    EM_ASM({
    Module.ctxs[$0].globalAlpha = $1;
    }, id, v);
}

void Renderer::round_line_cap() {
    EM_ASM({
    Module.ctxs[$0].lineCap = "round";
    }, id);
}

void Renderer::round_line_join() {
    EM_ASM({
    Module.ctxs[$0].lineJoin = "round";
    }, id);
}

static void update_transform(Renderer *r) {
EM_ASM({
    Module.ctxs[$0].setTransform($1, $2, $4, $5, $3, $6);
}, r->id, r->context.transform_matrix[0],r->context.transform_matrix[1],r->context.transform_matrix[2], 
r->context.transform_matrix[3],r->context.transform_matrix[4],r->context.transform_matrix[5]);
}

void Renderer::set_transform(float a, float b, float c, float d, float e, float f) {
    context.transform_matrix[0] = a;
    context.transform_matrix[1] = b;
    context.transform_matrix[2] = c;
    context.transform_matrix[3] = d;
    context.transform_matrix[4] = e;
    context.transform_matrix[5] = f;
    update_transform(this);
}

void Renderer::scale(float v) {
    context.transform_matrix[0] *= v;
    context.transform_matrix[1] *= v;
    context.transform_matrix[3] *= v;
    context.transform_matrix[4] *= v;
    update_transform(this);
}

void Renderer::scale(float x, float y) {
    context.transform_matrix[0] *= x;
    context.transform_matrix[1] *= x;
    context.transform_matrix[3] *= y;
    context.transform_matrix[4] *= y;
    update_transform(this);
}

void Renderer::translate(float x, float y) {
    context.transform_matrix[2] += x * context.transform_matrix[0];
    context.transform_matrix[5] += y * context.transform_matrix[4];
    update_transform(this);
}

void Renderer::rotate(float a) {
    float cos_a = cosf(a);
    float sin_a = sinf(a);
    float original0 = context.transform_matrix[0];
    float original1 = context.transform_matrix[1];
    float original3 = context.transform_matrix[3];
    float original4 = context.transform_matrix[4];
    context.transform_matrix[0] = original0 * cos_a + original1 * -sin_a;
    context.transform_matrix[1] = original0 * sin_a + original1 * cos_a;
    context.transform_matrix[3] = original3 * cos_a + original4 * -sin_a;
    context.transform_matrix[4] = original3 * sin_a + original4 * cos_a;
    update_transform(this);
}

void Renderer::reset_transform() {
    set_transform(1,0,0,0,1,0);
}

void Renderer::begin_path() {
EM_ASM({
    Module.ctxs[$0].beginPath();
}, id);
}

void Renderer::move_to(float x, float y) {
EM_ASM({
    Module.ctxs[$0].moveTo($1, $2);
}, id, x, y);
}

void Renderer::line_to(float x, float y) {
EM_ASM({
    Module.ctxs[$0].lineTo($1, $2);
}, id, x, y);
}

void Renderer::qcurve_to(float x, float y, float x1, float y1) {
EM_ASM({
    Module.ctxs[$0].quadraticCurveTo($1, $2, $3, $4);
}, id, x, y, x1, y1);
}

void Renderer::bcurve_to(float x, float y, float x1, float y1, float x2, float y2) {
EM_ASM({
    Module.ctxs[$0].bezierCurveTo($1, $2, $3, $4, $5, $6);
}, id, x, y, x1, y1, x2, y2);
}


void Renderer::partial_arc(float x, float y, float r, float sa, float ea, uint8_t ccw) {
EM_ASM({
    Module.ctxs[$0].arc($1, $2, $3, $4, $5, $6);
}, id, x, y, r, sa, ea, ccw);
}

void Renderer::arc(float x, float y, float r) {
    partial_arc(x, y, r, 0, 2 * M_PI, 0);
}

void Renderer::ellipse(float x, float y, float r1, float r2) {
EM_ASM({
    Module.ctxs[$0].ellipse($1, $2, $3, $4, 0, 2 * Math.PI, 0);
}, id, x, y, r1, r2);
}

void Renderer::fill_rect(float x, float y, float w, float h) {
EM_ASM({
    Module.ctxs[$0].fillRect($1, $2, $3, $4);
}, id, x, y, w, h);
}

void Renderer::stroke_rect(float x, float y, float w, float h) {
EM_ASM({
    Module.ctxs[$0].strokeRect($1, $2, $3, $4);
}, id, x, y, w, h);
}

void Renderer::rect(float x, float y, float w, float h) {
EM_ASM({
    Module.ctxs[$0].rect($1, $2, $3, $4);
}, id, x, y, w, h);
}

void Renderer::fill() {
EM_ASM({
    Module.ctxs[$0].fill();
}, id);
}

void Renderer::stroke() {
EM_ASM({
    Module.ctxs[$0].stroke();
}, id);
}

void Renderer::clip() {
EM_ASM({
    Module.ctxs[$0].clip();
}, id);
}