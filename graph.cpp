#include "features.h"

namespace features::graph
{
    void handle(ImDrawList* list)
    {
        if (!settings::visuals::enabled)
            return;

        if (!settings::visuals::graph)
            return;

        if (!g::engine_client->IsInGame())
            return;

        CCSPlayerController* localplayer = g::entity_system->GetLocalPlayerController<CCSPlayerController*>();
        if (!localplayer)
            return;

        CCSPlayerPawn* localpawn = g::entity_system->GetEntityFromHandle<CCSPlayerPawn*>(localplayer->m_hPlayerPawn());
        if (!localpawn)
            return;

        if (!localpawn->IsAlive())
            return;

        ImGuiIO& io = ImGui::GetIO();
        float position = (io.DisplaySize.y / 4.f + io.DisplaySize.y / 2.f - 30);

        auto velocity = localpawn->m_vecVelocity();

        static float velocity_history[100] = { 0 };
        static int velocity_index = 0;

        float current_velocity = velocity.length2D();
        velocity_history[velocity_index] = current_velocity;
        velocity_index = (velocity_index + 1) % 100;

        ImVec2 graph_size = ImVec2(200, 100);
        ImVec2 graph_position = ImVec2(io.DisplaySize.x / 2.f - graph_size.x / 2.f, position);
        float graph_bottom = graph_position.y + graph_size.y;
        float graph_top = graph_position.y;
        float graph_left = graph_position.x;
        float graph_right = graph_position.x + graph_size.x;

        auto catmull_rom_spline = [](float t, float p0, float p1, float p2, float p3) -> float
        {
            float t2 = t * t;
            float t3 = t2 * t;
            return 0.5f * ((2.0f * p1) + (-p0 + p2) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
        };

        for (int i = 1; i < 100 - 3; ++i)
        {
            int prev_index = (velocity_index + i - 1) % 100;
            int current_index = (velocity_index + i) % 100;
            int next_index = (velocity_index + i + 1) % 100;
            int next_next_index = (velocity_index + i + 2) % 100;

            float prev_velocity = velocity_history[prev_index];
            float current_velocity = velocity_history[current_index];
            float next_velocity = velocity_history[next_index];
            float next_next_velocity = velocity_history[next_next_index];

            float x1 = graph_left + (float(i - 1) / 100.0f) * graph_size.x;
            float x2 = graph_left + (float(i) / 100.0f) * graph_size.x;

            for (int j = 0; j < 10; ++j)
            {
                float t = j / 10.0f;
                float y1 = graph_bottom - (catmull_rom_spline(t, prev_velocity, current_velocity, next_velocity, next_next_velocity) / 500.0f) * graph_size.y;
                float y2 = graph_bottom - (catmull_rom_spline(t + 0.1f, prev_velocity, current_velocity, next_velocity, next_next_velocity) / 500.0f) * graph_size.y;

                y1 = std::clamp(y1, graph_top, graph_bottom);
                y2 = std::clamp(y2, graph_top, graph_bottom);

                float x = graph_left + (float(i) + t) / 100.0f * graph_size.x;

                list->AddLine(ImVec2(x, y1), ImVec2(x + 2.0f, y2), ImColor(225, 255, 255, 255), 1.0f);
            }

            if (current_velocity < prev_velocity)
            {
                float drop_x = graph_left + (float(i) / 100.0f) * graph_size.x;
                float drop_y = graph_bottom - (current_velocity / 500.0f) * graph_size.y;
                list->AddCircleFilled(ImVec2(drop_x, drop_y), 1.0f, ImColor(255, 0, 0, 255));
            };

            if (current_velocity > prev_velocity)
            {
                float drop_x = graph_left + (float(i) / 100.0f) * graph_size.x;
                float drop_y = graph_bottom - (current_velocity / 500.0f) * graph_size.y;
                list->AddCircleFilled(ImVec2(drop_x, drop_y), 1.0f, ImColor(0, 255, 0, 255));
            };
        };
    };
};