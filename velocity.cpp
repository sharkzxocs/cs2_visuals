#include "features.h"
#include "../sdk/helpers/xorstr.h"

namespace features::velocity
{
	void handle(ImDrawList* list)
	{
		if (!settings::visuals::enabled)
			return;

		if (!settings::visuals::velocity)
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

		auto& velocity = localpawn->m_vecVelocity();
		if (velocity.length2D() > 1000)
			return;

		char buffer[256];
		sprintf(buffer, xorstr_("Velocity: %1.f"), velocity.length2D());

		ImGuiIO& io = ImGui::GetIO();
		float position = (io.DisplaySize.y / 4.f + io.DisplaySize.y / 2.f + 90);

		ImVec2 text_size = ImGui::CalcTextSize(buffer);
		ImVec2 text_position = ImVec2(io.DisplaySize.x / 2.f - text_size.x / 2.f, position);

		list->AddText(text_position + ImVec2(1, 1), ImColor(0, 0, 0, 150), buffer);
		list->AddText(text_position, ImColor(225, 255, 255, 255), buffer);
	};
};