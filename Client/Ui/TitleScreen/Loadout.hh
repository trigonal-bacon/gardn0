#pragma once

#include <Client/Ui/Element.hh>


#include <stdint.h>
namespace ui {
    class TitleScreenLoadout : public Element {
    public:
        uint8_t pos;
        uint8_t render_id;
        TitleScreenLoadout(uint8_t);
        uint8_t get_dynamic_loadout_pos();

        virtual void on_render(Renderer &) override;
    };
}