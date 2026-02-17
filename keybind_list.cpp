#include "features.h"
#include "../sdk/helpers/xorstr.h"
#include "../thirdparty/ImGui/imgui_notify.h"

namespace features::keybind_list
{
    void handle(ImDrawList* list)
    {
        if (!settings::visuals::enabled || !settings::visuals::keybindlist)
            return;

        if (!interfaces::engine_client->IsInGame())
            return;

        ImVec2 screen_size = ImGui::GetIO().DisplaySize;
        float boxWidth = 240;
        float position = 400;
        float entryHeight = 20;
        float margin = 10.f;
        float right_position_x = screen_size.x - boxWidth - margin;

        const char* title = xorstr_("\xef\x84\x9c keybinds");
        ImVec2 title_size = ImGui::CalcTextSize(title);
        ImVec2 title_position = ImVec2(right_position_x + (boxWidth - title_size.x) / 2.0f, position + 5.f);

        ImVec2 rect_position1 = ImVec2(right_position_x, position);
        ImVec2 rect_position2 = ImVec2(rect_position1.x + boxWidth, rect_position1.y + title_size.y + 8.f);

        list->AddRectFilled(ImVec2(rect_position1.x, rect_position1.y), ImVec2(rect_position2.x, rect_position1.y + 2), ImU32(ImColor(settings::menu_settings::accent_color.x, settings::menu_settings::accent_color.y, settings::menu_settings::accent_color.z, 255 / 255.f)));
        list->AddRectFilled(rect_position1, rect_position2, ImColor(54, 57, 63, 100));

        ImVec2 shadow_offset = ImVec2(1.f, 1.f);
        list->AddText(title_position + shadow_offset, ImColor(0, 0, 0, 255), title);
        list->AddText(title_position, ImColor(255, 255, 255, 255), title);

        position += title_size.y + 10.f;

        enum KeyType { AIMBOT, TRIGGERBOT, MAGNET };

        std::vector<std::tuple<KeyType, int, bool&>> keybinds =
        {
            { AIMBOT, settings::legit::pistol::key, settings::legit::pistol::enabled },
            { AIMBOT, settings::legit::rifle::key, settings::legit::rifle::enabled },
            { AIMBOT, settings::legit::shotgun::key, settings::legit::shotgun::enabled },
            { AIMBOT, settings::legit::smg::key, settings::legit::smg::enabled },
            { AIMBOT, settings::legit::sniper::key, settings::legit::sniper::enabled },
            { TRIGGERBOT, settings::triggerbot_basic::pistol::key, settings::triggerbot_basic::pistol::enabled },
            { TRIGGERBOT, settings::triggerbot_basic::rifle::key, settings::triggerbot_basic::rifle::enabled },
            { TRIGGERBOT, settings::triggerbot_basic::shotgun::key, settings::triggerbot_basic::shotgun::enabled },
            { TRIGGERBOT, settings::triggerbot_basic::smg::key, settings::triggerbot_basic::smg::enabled },
            { TRIGGERBOT, settings::triggerbot_basic::sniper::key, settings::triggerbot_basic::sniper::enabled },
            { MAGNET, settings::triggerbot_experimental::pistol::key, settings::triggerbot_experimental::pistol::enabled },
            { MAGNET, settings::triggerbot_experimental::rifle::key, settings::triggerbot_experimental::rifle::enabled },
            { MAGNET, settings::triggerbot_experimental::shotgun::key, settings::triggerbot_experimental::shotgun::enabled },
            { MAGNET, settings::triggerbot_experimental::smg::key, settings::triggerbot_experimental::smg::enabled },
            { MAGNET, settings::triggerbot_experimental::sniper::key, settings::triggerbot_experimental::sniper::enabled }
        };

        bool active[3] = { false, false, false };

        for (const auto& [type, key, enabled] : keybinds)
        {
            if (enabled && (LI_FN(GetAsyncKeyState)(key) & 0x8000))
            {
                active[type] = true;
            }
        }

        if (active[AIMBOT]) {
            list->AddText(ImVec2(right_position_x + 5.f, position), ImColor(255, 255, 255, 255), xorstr_("aimbot [enabled]"));
            position += entryHeight;
        }
        if (active[TRIGGERBOT]) {
            list->AddText(ImVec2(right_position_x + 5.f, position), ImColor(255, 255, 255, 255), xorstr_("triggerbot [enabled]"));
            position += entryHeight;
        }
        if (active[MAGNET]) {
            list->AddText(ImVec2(right_position_x + 5.f, position), ImColor(255, 255, 255, 255), xorstr_("magnet [enabled]"));
            position += entryHeight;
        }
    }
};