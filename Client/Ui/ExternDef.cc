#include <Client/Ui/Ui.hh>

#include <Client/Input.hh>

namespace ui {
    Window *g_window = nullptr;
    Element *g_focused = nullptr;
    float g_scale = 1;
    float g_frame_dt = 16.67;
    uint8_t panel_open = PanelOpen::kNone;
};

Renderer *ui::g_renderer = nullptr;
Input *ui::g_input = nullptr;
