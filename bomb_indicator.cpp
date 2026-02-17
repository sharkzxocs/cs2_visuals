#include "features.h"
#include "../sdk/helpers/xorstr.h"
#include "../render/fonts.h"
#include <fmt/core.h>

#define IM_PI 3.14159265358979323846

namespace features::bomb_indicator
{
    void handle(ImDrawList* list)
    {
        if (!settings::visuals::enabled)
            return;

        if (!settings::visuals::bomb_indicator)
            return;

        if (!g::engine_client->IsInGame())
            return;

        if (!interfaces::game_rules->m_bBombPlanted())
            return;

        for (int i = 0; i < 1024; i++)
        {
            auto* entity = g::entity_system->GetBaseEntity(i);
            if (!entity || !entity->m_pEntity()->IsValid() || !entity->m_pEntity()->GetEntityIndex() || !entity->IsBomb())
                continue;

            if (entity->m_bHasExploded())
                continue;

            if (!entity->m_bBombTicking())
                continue;

            const ImGuiIO& io = ImGui::GetIO();

            auto rect_position1 = ImVec2(io.DisplaySize.x / 2.f - 100.f, io.DisplaySize.y / 2.f - 30.f - 350.f);
            auto rect_position2 = ImVec2(io.DisplaySize.x / 2.f + 100.f, io.DisplaySize.y / 2.f + 30.f - 350.f);

            list->AddRectFilled(ImVec2(rect_position1.x, rect_position1.y), ImVec2(rect_position2.x, rect_position1.y + 2), ImU32(ImColor(settings::menu_settings::accent_color.x, settings::menu_settings::accent_color.y, settings::menu_settings::accent_color.z, 255 / 255.f)));
            list->AddRectFilled(rect_position1, rect_position2, ImColor(54, 57, 63, 100));

            constexpr float circle_radius = 20.f;
            constexpr float circle_thickness = 5.0f;

            auto circle_center = ImVec2(rect_position1.x + circle_radius + 10, (rect_position1.y + rect_position2.y) / 2.0f);

            const auto bomb_time = std::max(entity->m_flC4Blow() - interfaces::global_vars->m_curtime, 0.0f);
            if (!bomb_time)
                continue;

            constexpr float total_bomb_duration = 40.0f;

            constexpr float start_angle = -IM_PI / 2;
            const float fraction = std::clamp(bomb_time / total_bomb_duration, 0.0f, 1.0f);
            const float end_angle = start_angle + IM_PI * 2 * fraction;

            list->AddCircle(circle_center, circle_radius, ImColor(64, 64, 64, 200), 64, circle_thickness);

            list->PathArcTo(circle_center, circle_radius, start_angle, end_angle, 64);
            list->PathStroke(static_cast<ImU32>(ImColor(settings::menu_settings::accent_color.x, settings::menu_settings::accent_color.y,
                settings::menu_settings::accent_color.z, 255 / 255.f)), false, circle_thickness);

            char time_text[16];
            snprintf(time_text, sizeof(time_text), xorstr_("%.1f"), bomb_time);
            ImVec2 time_text_size = ImGui::CalcTextSize(time_text);
            list->AddText(ImVec2(circle_center.x - time_text_size.x / 2 + ImVec2(1, 1).x, circle_center.y - time_text_size.y / 2 + +ImVec2(1, 1).y), ImColor(0, 0, 0, 255), time_text);
            list->AddText(ImVec2(circle_center.x - time_text_size.x / 2, circle_center.y - time_text_size.y / 2), ImColor(255, 255, 255, 255), time_text);

            std::string bomb_site;
            entity->m_nBombSite() == 0 ? bomb_site = xorstr_("A") : bomb_site = xorstr_("B");

            auto defuse_time = entity->m_flDefuseCountDown() - interfaces::global_vars->m_curtime;
            if (!defuse_time)
                continue;

            std::string status;
            if (entity->m_bBeingDefused())
                status = fmt::format(xorstr_("Defusing: ({:.1f}s)"), defuse_time);
            else if (!entity->m_bBeingDefused())
            {
                status = xorstr_("Planted");
            }

            auto text_position = ImVec2(circle_center.x + circle_radius + 20.f, rect_position1.y + 10.f);

            char bomb_site_text[32];
            snprintf(bomb_site_text, sizeof(bomb_site_text), xorstr_("Bomb site: %s"), bomb_site.c_str());
            list->AddText(text_position + ImVec2(1.f, 1.f), ImColor(0, 0, 0, 255), bomb_site_text);
            list->AddText(text_position, ImColor(255, 255, 255, 255), bomb_site_text);

            text_position.y += 20;
            char status_text[32];
            snprintf(status_text, sizeof(status_text), xorstr_("%s"), status.c_str());
            list->AddText(text_position + ImVec2(1.f, 1.f), ImColor(0, 0, 0, 255), status_text);
            list->AddText(text_position, ImColor(255, 255, 255, 255), status_text);
        }
    }
};