#pragma once

#include <Client/Ui/Element.hh>

namespace ui {
    class LoadoutPetalBackground : public Element {
    public:
        uint8_t pos;
        LoadoutPetalBackground(uint8_t, uint8_t);
        uint8_t get_dynamic_loadout_pos();

        virtual void on_render(Renderer &) override;
    };

    class TrashPetalBackground : public LoadoutPetalBackground {
    public:
        TrashPetalBackground();
    };

    class LoadoutPetalButton : public Element {
    public:
        uint8_t pos;
        uint8_t prev_id = 0;
        uint8_t render_id = 0;
        LoadoutPetalBackground *snap_to = nullptr;
        LerpValue reload;
        LerpValue angle;
        LoadoutPetalButton(uint8_t);
        uint8_t get_static_loadout_pos();

        virtual void on_render(Renderer &) override;
        virtual void on_render_skip() override;

        virtual void on_event(uint8_t) override;
    };

    class LoadoutPetalStandIn : public Element {
    public:
        LoadoutPetalStandIn();
        virtual void on_render(Renderer &) override;
    };

    void loadout_poll_input();
    Element *in_game_loadout_init();
}