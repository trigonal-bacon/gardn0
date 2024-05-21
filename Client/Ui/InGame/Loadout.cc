#include <Client/Ui/InGame/Loadout.hh>

#include <Client/Ui/VisualUtils.hh>

#include <Client/Assets/Assets.hh>

#include <Client/Game.hh>
#include <Shared/StaticData.hh>

#include <iostream>

using namespace ui;

static LoadoutPetalBackground *petal_backgrounds[2 * MAX_SLOT_COUNT + 1] = {nullptr};
static LoadoutPetalButton *petal_buttons[2 * MAX_SLOT_COUNT] = {nullptr};
static LoadoutPetalButton *petal_button_focused = nullptr;
static LoadoutPetalButton *petal_button_focused_by_keypad = nullptr;

LoadoutPetalBackground::LoadoutPetalBackground(uint8_t static_pos, uint8_t secondary) {
    should_render = [=](){
        return secondary || (static_pos < gardn->cache_slot_count);
    };
    width = height = secondary ? 60 : 75;
    static_pos = MAX_SLOT_COUNT * secondary + static_pos;
    pos = static_pos;
    ui::AddTheme(
        ui::VisualData::PanelBackground(0xffcfcfcf),
        this
    );
    petal_backgrounds[pos] = this;
}

uint8_t LoadoutPetalBackground::get_dynamic_loadout_pos() {
    //should NOT depend on player existing to return a consistent value
    if (pos >= MAX_SLOT_COUNT) return gardn->cache_slot_count + (pos - MAX_SLOT_COUNT);
    return pos; //fix
}

TrashPetalBackground::TrashPetalBackground() : LoadoutPetalBackground(MAX_SLOT_COUNT, 1) {
    ui::AddTheme(
        ui::VisualData::PanelBackground(0xffcf8888),
        this
    );
    width = height = 60;
}

void LoadoutPetalBackground::on_render(Renderer &ctx) {
    uint32_t color = visuals.fill;
    ctx.set_fill(Renderer::HSV(color, 0.8));
    ctx.scale(width / 60);
    DRAW_LOADOUT_BG(color);
}

LoadoutPetalButton::LoadoutPetalButton(uint8_t dynamic_pos) {
    pos = dynamic_pos;
    should_render = [&](){
        if (!gardn->simulation_ready) return false;
        //assume camera exists (it has to)
        if (gardn->simulation.get_ent(gardn->camera_id).state_per_loadout_ids[this->pos]) this->prev_id = gardn->simulation.get_ent(gardn->camera_id).loadout_ids[this->pos];
        return (this->prev_id != PetalId::kNone);
    };
    ui::AddTheme(
        ui::VisualData::PanelBackground(0xffcfcfcf),
        this
    );
    petal_buttons[pos] = this;
    detached = 1; //100% detached
    prev_id = PetalId::kNone;

    animate = [](Element *_self, Renderer &ctx) {
        LoadoutPetalButton *self = dynamic_cast<LoadoutPetalButton *>(_self);
        float _d = 1 - (pow(1 - 0.2, g_frame_dt * 60 / 1000));
        if (self != petal_button_focused) {
            uint8_t pos_to = self->get_static_loadout_pos();
            LoadoutPetalBackground *to = petal_backgrounds[pos_to];
            if (self->snap_to == petal_backgrounds[2 * MAX_SLOT_COUNT] && self->prev_id == PetalId::kNone) to = self->snap_to;
            LERP(self->x, to->screen_x, _d);
            LERP(self->y, to->screen_y, _d);
            if (self == petal_button_focused_by_keypad) {
                LERP(self->width, 65, _d);
                LERP(self->height, 65, _d);
            }
            else {
                LERP(self->width, to->width, _d);
                LERP(self->height, to->height, _d);
            }
        }
        else {
            if (!BIT_AT(g_input->mouse_buttons_state, 0)) {
                petal_button_focused = nullptr;
                if (self->snap_to != nullptr) {
                    //trash_bg
                    if (self->snap_to == petal_backgrounds[2 * MAX_SLOT_COUNT]) {
                        //send_packet
                        uint8_t packet[16];
                        Writer writer(static_cast<uint8_t *>(packet));
                        writer.write_uint8(kServerbound::kPetalDelete);
                        writer.write_uint8(self->pos);
                        gardn->socket.send(writer.packet, writer.at - writer.packet);
                        self->prev_id = PetalId::kNone;
                        //self->snap_to = nullptr;
                    }
                    else {
                        //swap logic (pain)
                        uint8_t pos1 = self->pos;
                        uint8_t pos2 = self->snap_to->get_dynamic_loadout_pos();
                        LoadoutPetalButton *first = petal_buttons[pos1];
                        LoadoutPetalButton *second = petal_buttons[pos2];
                        uint8_t id1 = first->prev_id;
                        uint8_t id2 = second->prev_id;
                        second->prev_id = id1;
                        first->prev_id = id2;
                        second->render_id = second->prev_id;
                        first->render_id = first->prev_id;
                        first->render_animation.lerp_value = first->render_animation.value = first->prev_id != PetalId::kNone;
                        second->render_animation.lerp_value = second->render_animation.value = second->prev_id != PetalId::kNone;
                        second->x = first->x;
                        second->y = first->y;
                        second->width = first->width;
                        second->height = first->height;
                        uint8_t packet[16];
                        Writer writer(static_cast<uint8_t *>(packet));
                        writer.write_uint8(kServerbound::kPetalSwap);
                        writer.write_uint8(pos1);
                        writer.write_uint8(pos2);
                        gardn->socket.send(writer.packet, writer.at - writer.packet);
                        self->snap_to = nullptr;
                    }
                }
            }
            else {
                //possibly stick to another (logic later)
                self->snap_to = nullptr;
                for (uint8_t i = 0; i < gardn->cache_slot_count; ++i) {
                    if (i == self->pos) continue;
                    LoadoutPetalBackground *potential = petal_backgrounds[i];
                    if (fabsf(g_input->mouse_x - potential->screen_x) < potential->width * g_scale / 2
                    && fabsf(g_input->mouse_y - potential->screen_y) < potential->height * g_scale / 2) {
                        self->snap_to = potential;
                        break;
                    }
                }
                if (self->snap_to == nullptr) {
                    for (uint8_t i = 0; i < MAX_SLOT_COUNT + 1; ++i) {
                        if (i + gardn->cache_slot_count == self->pos) continue;
                        LoadoutPetalBackground *potential = petal_backgrounds[i + MAX_SLOT_COUNT];
                        if (fabsf(g_input->mouse_x - potential->screen_x) < potential->width * g_scale / 2
                        && fabsf(g_input->mouse_y - potential->screen_y) < potential->height * g_scale / 2) {
                            self->snap_to = potential;
                            break;
                        }
                    }
                }
                if (self->snap_to == nullptr) {
                    LERP(self->x, g_input->mouse_x, _d);
                    LERP(self->y, g_input->mouse_y, _d);
                    LERP(self->width, (self->pos < gardn->cache_slot_count ? 85 : 65), _d);
                    LERP(self->height, (self->pos < gardn->cache_slot_count ? 85 : 65), _d);
                }
                else {
                    LoadoutPetalBackground *to = self->snap_to;
                    LERP(self->x, to->screen_x, _d);
                    LERP(self->y, to->screen_y, _d);
                    LERP(self->width, to->width, _d);
                    LERP(self->height, to->height, _d);
                }
            }
        }
        if (self->prev_id != PetalId::kNone) {
            //self->snap_to = nullptr;
            float rld = gardn->simulation.get_ent(gardn->camera_id).loadout_reloads[self->pos];
            if (rld < self->reload) self->reload.lerp_value = self->reload.value = rld;
            else {
                self->reload = rld;
                self->reload.lerp_step(_d);
            }
            self->render_id = self->prev_id;
        }    
        if (petal_button_focused == self || petal_button_focused_by_keypad == self) {
            self->angle = 0.1 * sin(gardn->curr_tick * 6.28 / 1000);
        }
        else self->angle = 0;
        self->angle.lerp_step(_d);
    };
}

uint8_t LoadoutPetalButton::get_static_loadout_pos() {
    //should NOT depend on player existing to return a consistent value
    if (pos >= gardn->cache_slot_count) return MAX_SLOT_COUNT + (pos - gardn->cache_slot_count);
    return pos; //fix
}

void LoadoutPetalButton::on_render(Renderer &ctx) {
    if (render_id == PetalId::kNone) return;
    screen_x = x;
    screen_y = y;
    RenderContext context(&ctx);
    ctx.reset_transform();
    ctx.translate(x, y);
    ctx.scale(g_scale);
    ctx.scale(render_animation);
    ctx.rotate((float) angle);
    {
        uint32_t color = RARITY_COLORS[PETAL_DATA[render_id].rarity];
        ctx.scale(width / 60);
        DRAW_LOADOUT_BG(color);
        if (pos >= gardn->cache_slot_count) {

        }
        else {
            RenderContext context(&ctx);
            ctx.clip();
            float pct = 1 - reload / 255;
            pct = pct * pct * pct * (pct * (6.0f * pct - 15.0f) + 10.0f);
            ctx.set_fill(0x40000000);
            ctx.begin_path();
            ctx.move_to(0,0);
            ctx.partial_arc(0, 0, 90, -M_PI / 2 - pct * M_PI * 10, -M_PI / 2 - pct * M_PI * 8, 0);
            ctx.fill();
        }
        //do the reloaed thing
        /*
        rr_renderer_partial_arc(renderer, 0, 0, 90, -M_PI / 2 - pct * M_PI * 10,
                                -M_PI / 2 - pct * M_PI * 8, 0);
                                */
    }
    ctx.translate(0, -5);
    draw_static_petal(render_id, ctx);
    ctx.translate(0, 20);
    ctx.set_text_size(12);
    ctx.set_line_width(12 * 0.12);
    ctx.set_fill(0xffffffff);
    ctx.set_stroke(0xff222222);
    ctx.center_text_align();
    ctx.center_text_baseline();
    ctx.stroke_text(PETAL_DATA[render_id].name);
    ctx.fill_text(PETAL_DATA[render_id].name);
}

void LoadoutPetalButton::on_render_skip() {
    x = g_renderer->width / 2;
    y = g_renderer->height / 2;
}

void LoadoutPetalButton::on_event(uint8_t type) {
    if (type == kMouseDown) { petal_button_focused = this; petal_button_focused_by_keypad = nullptr; }
}

LoadoutPetalStandIn::LoadoutPetalStandIn() : Element () {}

void LoadoutPetalStandIn::on_render(Renderer &ctx) {
    if (petal_button_focused != nullptr) petal_button_focused->on_render(ctx);
}

static double last_input = 0;
static uint8_t focused = 0;
static uint8_t button_index_at = 0;
void ui::loadout_poll_input() {
    if (petal_button_focused_by_keypad != nullptr && (petal_button_focused_by_keypad->pos < gardn->cache_slot_count || gardn->cache_loadout[petal_button_focused_by_keypad->pos] == PetalId::kNone)) { petal_button_focused_by_keypad = nullptr; focused = 0; button_index_at = 0; }
    if (gardn->input.keys_pressed_this_tick.contains('E')) {
        uint8_t *secondary_loadout = &gardn->simulation.get_ent(gardn->camera_id).loadout_ids[gardn->cache_slot_count];
        for (uint8_t offset = focused != 0; offset < MAX_SLOT_COUNT; ++offset) {
            if (secondary_loadout[(offset + button_index_at) % MAX_SLOT_COUNT] == PetalId::kNone) continue;
            button_index_at = (offset + button_index_at) % MAX_SLOT_COUNT;
            focused = 1;
            last_input = gardn->curr_tick;
            petal_button_focused_by_keypad = petal_buttons[gardn->cache_slot_count + button_index_at];
            break;
        }
    }
    else if (gardn->input.keys_pressed_this_tick.contains('Q')) {
        uint8_t *secondary_loadout = &gardn->simulation.get_ent(gardn->camera_id).loadout_ids[gardn->cache_slot_count];
        for (uint8_t offset = focused != 0; offset < MAX_SLOT_COUNT; ++offset) {
            if (secondary_loadout[(button_index_at - offset + MAX_SLOT_COUNT) % MAX_SLOT_COUNT] == PetalId::kNone) continue;
            button_index_at = (button_index_at - offset + MAX_SLOT_COUNT) % MAX_SLOT_COUNT;
            focused = 1;
            last_input = gardn->curr_tick;
            petal_button_focused_by_keypad = petal_buttons[gardn->cache_slot_count + button_index_at];
            break;
        }
    }
    else {
        if (petal_button_focused_by_keypad != nullptr) {
            for (uint8_t i = 0; i < gardn->cache_slot_count; ++i) {
                if (!gardn->input.keys_pressed_this_tick.contains('1'+i)) continue;
                LoadoutPetalButton *self = petal_button_focused_by_keypad;
                uint8_t pos1 = self->pos;
                uint8_t pos2 = i;
                LoadoutPetalButton *first = petal_buttons[pos1];
                LoadoutPetalButton *second = petal_buttons[pos2];
                uint8_t id1 = first->prev_id;
                uint8_t id2 = second->prev_id;
                second->prev_id = id1;
                first->prev_id = id2;
                second->render_id = second->prev_id;
                first->render_id = first->prev_id;
                first->render_animation.lerp_value = first->render_animation.value = first->prev_id != PetalId::kNone;
                second->render_animation.lerp_value = second->render_animation.value = second->prev_id != PetalId::kNone;
                float temp = second->x;
                second->x = first->x;
                first->x = temp;
                temp = second->y;
                second->y = first->y;
                first->y = temp;
                second->width = first->width;
                second->height = first->height;
                uint8_t packet[16];
                Writer writer(static_cast<uint8_t *>(packet));
                writer.write_uint8(kServerbound::kPetalSwap);
                writer.write_uint8(pos1);
                writer.write_uint8(pos2);
                gardn->socket.send(writer.packet, writer.at - writer.packet);
                self->snap_to = nullptr;
                last_input = gardn->curr_tick;
                break;
            }
        }
        if (gardn->input.keys_pressed_this_tick.contains('T') && petal_button_focused_by_keypad != nullptr) {
            petal_button_focused_by_keypad->snap_to = petal_backgrounds[2 * MAX_SLOT_COUNT];
            uint8_t packet[16];
            Writer writer(static_cast<uint8_t *>(packet));
            writer.write_uint8(kServerbound::kPetalDelete);
            writer.write_uint8(petal_button_focused_by_keypad->pos);
            gardn->socket.send(writer.packet, writer.at - writer.packet);
            petal_button_focused_by_keypad->prev_id = PetalId::kNone;
            petal_button_focused_by_keypad = nullptr;
            uint8_t *secondary_loadout = &gardn->simulation.get_ent(gardn->camera_id).loadout_ids[gardn->cache_slot_count];
            for (uint8_t offset = 1; offset < MAX_SLOT_COUNT; ++offset) {
                if (secondary_loadout[(offset + button_index_at) % MAX_SLOT_COUNT] == PetalId::kNone) continue;
                button_index_at = (offset + button_index_at) % MAX_SLOT_COUNT;
                focused = 1;
                last_input = gardn->curr_tick;
                petal_button_focused_by_keypad = petal_buttons[gardn->cache_slot_count + button_index_at];
                return;
            }
        }
        if (gardn->curr_tick - last_input > 5000) { petal_button_focused_by_keypad = nullptr; focused = 0; button_index_at = 0; }
    }
}

Element *ui::in_game_loadout_init() {
    return ui::SetRenderCondition(
        [](){ return gardn->simulation_ready && gardn->alive(); },
        ui::Justify<0, 1>(
            (new ui::VContainer{
                (new ui::HContainer{
                    new ui::LoadoutPetalBackground(0, 0),
                    new ui::LoadoutPetalBackground(1, 0),
                    new ui::LoadoutPetalBackground(2, 0),
                    new ui::LoadoutPetalBackground(3, 0),
                    new ui::LoadoutPetalBackground(4, 0),
                    new ui::LoadoutPetalBackground(5, 0),
                    new ui::LoadoutPetalBackground(6, 0),
                    new ui::LoadoutPetalBackground(7, 0),
                })->pad(0,10),
                (new ui::HContainer{
                    new ui::LoadoutPetalBackground(0, 1),
                    new ui::LoadoutPetalBackground(1, 1),
                    new ui::LoadoutPetalBackground(2, 1),
                    new ui::LoadoutPetalBackground(3, 1),
                    new ui::LoadoutPetalBackground(4, 1),
                    new ui::LoadoutPetalBackground(5, 1),
                    new ui::LoadoutPetalBackground(6, 1),
                    new ui::LoadoutPetalBackground(7, 1),
                    new ui::TrashPetalBackground(),
                })->pad(0,10),
                new ui::LoadoutPetalButton(0),
                new ui::LoadoutPetalButton(1),
                new ui::LoadoutPetalButton(2),
                new ui::LoadoutPetalButton(3),
                new ui::LoadoutPetalButton(4),
                new ui::LoadoutPetalButton(5),
                new ui::LoadoutPetalButton(6),
                new ui::LoadoutPetalButton(7),
                new ui::LoadoutPetalButton(8),
                new ui::LoadoutPetalButton(9),
                new ui::LoadoutPetalButton(10),
                new ui::LoadoutPetalButton(11),
                new ui::LoadoutPetalButton(12),
                new ui::LoadoutPetalButton(13),
                new ui::LoadoutPetalButton(14),
                new ui::LoadoutPetalButton(15),
                new ui::LoadoutPetalStandIn()
            })->pad(0,10)
        )
    );
}