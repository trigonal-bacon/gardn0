#pragma once

class Renderer;
class Input;

namespace ui {
    class Element;
    class Window;

    extern Window *g_window;
    extern Element *g_focused;
    extern Renderer *g_renderer;
    extern Input *g_input;
    extern float g_scale;
    extern float g_frame_dt;
}