#include "features.h"
#include "../sdk/helpers/xorstr.h"

namespace features::key_strokes
{
    void handle(ImDrawList* list)
    {
        if (!settings::visuals::enabled || !settings::visuals::key_strokes || !g::engine_client->IsInGame())
            return;

        ImGuiIO& io = ImGui::GetIO();
        float center_x = io.DisplaySize.x / 2.f;
        float center_y = io.DisplaySize.y / 2.f + 160;

        const float box_size = 25.0f;
        const float space_width = 70.0f; 
        const float spacing = 4.0f;

        auto draw_box = [&](const char* keyText, float x, float y, float width, float height, bool isActive)
        {
            ImColor box_color = isActive
                ? ImColor(settings::visuals::keystrokes_color.x, settings::visuals::keystrokes_color.y, settings::visuals::keystrokes_color.z, 0.2f)
                : ImColor(0, 0, 0, 0.2);

            ImColor text_color = ImColor(255, 255, 255, 255);

            ImVec2 boxMin(x - width / 2, y - height / 2);
            ImVec2 boxMax(x + width / 2, y + height / 2);
            ImVec2 textPos(x - ImGui::CalcTextSize(keyText).x / 2, y - ImGui::CalcTextSize(keyText).y / 2);

            list->AddRectFilled(boxMin, boxMax, box_color);
            list->AddRect(boxMin, boxMax, ImColor(255, 255, 255, 100));
            list->AddText(textPos, ImU32(text_color), keyText);
        };

        bool w_active = LI_FN(GetAsyncKeyState)('W');
        bool a_active = LI_FN(GetAsyncKeyState)('A');
        bool s_active = LI_FN(GetAsyncKeyState)('S');
        bool d_active = LI_FN(GetAsyncKeyState)('D');
        bool space_active = LI_FN(GetAsyncKeyState)(VK_SPACE);

        float row_y = center_y;

        float w_x = center_x;
        float w_y = row_y - (box_size + spacing);

        float a_x = center_x - (box_size + spacing);
        float s_x = center_x;
        float d_x = center_x + (box_size + spacing);

        float space_x = center_x;
        float space_y = row_y + (box_size + spacing);

        draw_box("W", w_x, w_y, box_size, box_size, w_active);
        draw_box("A", a_x, row_y, box_size, box_size, a_active);
        draw_box("S", s_x, row_y, box_size, box_size, s_active);
        draw_box("D", d_x, row_y, box_size, box_size, d_active);
        draw_box("SPACE", space_x, space_y, space_width, box_size, space_active);
    }
};