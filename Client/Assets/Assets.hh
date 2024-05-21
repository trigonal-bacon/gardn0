#pragma once

#include <Client/Render/Renderer.hh>

#include <stdint.h>

#define DRAW_LOADOUT_BG(color) \
{ \
    ctx.set_fill(Renderer::HSV(color, 1.0)); \
    ctx.set_stroke(Renderer::HSV(color, 0.8)); \
    ctx.set_line_width(8); \
    ctx.round_line_join(); \
    ctx.round_line_cap(); \
    ctx.begin_path(); \
    ctx.rect(-26, -26, 52, 52); \
    ctx.stroke(); \
    ctx.fill(); \
}

void draw_static_petal_single(uint8_t, Renderer &);

void draw_static_petal(uint8_t, Renderer &);

void draw_static_mob(uint8_t, Renderer &, float, float, uint32_t);