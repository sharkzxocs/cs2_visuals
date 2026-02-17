#include "features.h"
#include "../communication/client_handler.h"
#include "../sdk/helpers/xorstr.h"
#include "../sdk/helpers/receive_data.h"
#include "../thirdparty/ImGui/imgui.h"
#include "../thirdparty/ImGui/imgui_internal.h"

namespace features::watermark
{
    void handle(ImDrawList* list)
    {
        if (!settings::visuals::watermark)
            return;

        char buffer[256];
        sprintf_s(buffer, xorstr_("luftwaffe | luigi pasta"));

        const auto watermark_size = ImGui::CalcTextSize(buffer);

        constexpr float y_offset = 0;

        list->AddRectFilled(ImVec2(6, 6 + y_offset), ImVec2((4 * 2) + watermark_size.x + 6, 8 + y_offset), static_cast<ImU32>(ImColor(settings::menu_settings::accent_color.x, settings::menu_settings::accent_color.y, settings::menu_settings::accent_color.z, 255 / 255.f)));

        list->AddRectFilled(ImVec2(6, 6 + y_offset), ImVec2((4 * 2) + watermark_size.x + 6, 27 + y_offset), ImColor(54, 57, 63, 100));

        list->AddText(ImVec2(10, 6 + y_offset) + ImVec2(1, 1), ImColor(0, 0, 0, 255), buffer);

        const char* accent_start = xorstr_("luftwaffe");
        const char* remaining_text = buffer + strlen(accent_start);

        list->AddText(ImVec2(10, 6 + y_offset), static_cast<ImU32>(ImColor(settings::menu_settings::accent_color.x, settings::menu_settings::accent_color.y, settings::menu_settings::accent_color.z, 255 / 255.f)), accent_start);

        list->AddText(ImVec2(10 + ImGui::CalcTextSize(accent_start).x, 6 + y_offset), ImColor(255, 255, 255, 255), remaining_text);
    };
};