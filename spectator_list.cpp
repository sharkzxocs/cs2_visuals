#include "features.h"
#include "../sdk/helpers/xorstr.h"
#include "../thirdparty/ImGui/imgui_notify.h"

namespace features::spectator_list
{
    ID3D11ShaderResourceView* CreateAvatarTexture(ID3D11Device* device, ID3D11DeviceContext* context, const CSteamID& steamID)
    {
        int iImage = interfaces::steam_friends->GetSmallFriendAvatar(steamID);
        if (iImage == -1)
            return nullptr;

        uint32 uAvatarWidth, uAvatarHeight;
        if (!interfaces::steam_utils->GetImageSize(iImage, &uAvatarWidth, &uAvatarHeight))
            return nullptr;

        const int uImageSizeInBytes = uAvatarWidth * uAvatarHeight * 4;
        uint8* pAvatarRGBA = new uint8[uImageSizeInBytes];
        if (!interfaces::steam_utils->GetImageRGBA(iImage, pAvatarRGBA, uImageSizeInBytes))
        {
            delete[] pAvatarRGBA;
            return nullptr;
        }

        std::vector<uint8_t> texData;
        texData.resize(uAvatarWidth * uAvatarHeight * 4u);

        for (size_t i = 0; i < uAvatarWidth * uAvatarHeight; ++i)
        {
            uint32_t* pixel = reinterpret_cast<uint32_t*>(&pAvatarRGBA[i * 4]);
            uint32_t color = *pixel;

            uint32_t swappedColor = (color & 0xFF00FF00) | ((color & 0xFF) << 16) | ((color & 0xFF0000) >> 16);

            reinterpret_cast<uint32_t*>(&texData[i * 4])[0] = swappedColor;
        }

        ID3D11Texture2D* texture = nullptr;
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = uAvatarWidth;
        texDesc.Height = uAvatarHeight;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = texData.data();
        initData.SysMemPitch = uAvatarWidth * 4;

        HRESULT hr = device->CreateTexture2D(&texDesc, &initData, &texture);
        if (FAILED(hr))
        {
            delete[] pAvatarRGBA;
            return nullptr;
        }

        ID3D11ShaderResourceView* srv = nullptr;
        hr = device->CreateShaderResourceView(texture, nullptr, &srv);
        if (FAILED(hr))
        {
            texture->Release();
            delete[] pAvatarRGBA;
            return nullptr;
        }

        texture->Release();
        delete[] pAvatarRGBA;

        return srv;
    }

    void handle(ID3D11Device* device, ID3D11DeviceContext* context)
    {
        if (!settings::visuals::enabled || !settings::visuals::spectatorlist)
            return;

        static std::unordered_map<uint64, ID3D11ShaderResourceView*> avatarCache;

        if (!interfaces::engine_client->IsInGame())
        {
            avatarCache.clear();
            return;
        }

        CCSPlayerController* local_player = g::entity_system->GetLocalPlayerController<CCSPlayerController*>();
        if (!local_player)
            return;

        CCSPlayerPawn* local_pawn = g::entity_system->GetEntityFromHandle<CCSPlayerPawn*>(local_player->m_hPlayerPawn());
        if (!local_pawn)
            return;

        auto* list = ImGui::GetBackgroundDrawList();
        float position = 400;
        float boxWidth = 240;
        float entryHeight = 20;
        const char* title = xorstr_("\xef\x81\xae spectators");
        ImVec2 title_size = ImGui::CalcTextSize(title);
        float title_position_x = 10.f + (boxWidth - title_size.x) / 2.0f;
        ImVec2 title_position = ImVec2(title_position_x, position + 5.f);

        ImVec2 rect_position1 = ImVec2(10.f, position);
        ImVec2 rect_position2 = ImVec2(rect_position1.x + boxWidth, rect_position1.y + title_size.y + 8.f);

        list->AddRectFilled(ImVec2(rect_position1.x, rect_position1.y), ImVec2(rect_position2.x, rect_position1.y + 2), ImU32(ImColor(settings::menu_settings::accent_color.x, settings::menu_settings::accent_color.y, settings::menu_settings::accent_color.z, 255 / 255.f)));
        list->AddRectFilled(rect_position1, rect_position2, ImColor(54, 57, 63, 100));
        
        ImVec2 shadow_offset = ImVec2(1.f, 1.f);
        list->AddText(title_position + shadow_offset, ImColor(0, 0, 0, 255), title);
        list->AddText(title_position, ImColor(255, 255, 255, 255), title);

        position += title_size.y + 10.f;

        for (int i = 1; i < 65; i++)
        {
            auto* entity = reinterpret_cast<CCSPlayerController*>(g::entity_system->GetBaseEntity(i));
            if (!entity || !entity->IsController())
                continue;

            auto* pawn = g::entity_system->GetEntityFromHandle<CCSPlayerPawn*>(entity->m_hPawn());
            if (!pawn)
                continue;

            if (pawn->IsAlive())
                continue;

            auto pawn_obs = pawn->m_pObserverServices();
            if (!pawn_obs)
                continue;

            CCSPlayerController* observer_controller = reinterpret_cast<CCSPlayerController*>(g::entity_system->GetEntityFromHandle(pawn_obs->m_hObserverTarget()));
            if (!observer_controller)
                continue;

            CCSPlayerPawn* observer_pawn = reinterpret_cast<CCSPlayerPawn*>(observer_controller);
            if (!observer_pawn)
                continue;

            if (observer_pawn == local_pawn)
            {
                const auto steamID = entity->m_steamID();
                if (!steamID)
                    continue;

                const char* playerName = interfaces::steam_friends->GetFriendPersonaName(steamID);

                std::string displayedName = (playerName && *playerName) ? playerName : xorstr_("Bot");

                auto& avatarTexture = avatarCache[steamID];
                if (!avatarTexture)
                {
                    avatarTexture = CreateAvatarTexture(device, context, steamID);
                }

                const float avatarSize = 20.f;
                const float maxNameWidth = boxWidth - avatarSize - 15.0f;

                std::string truncatedName = displayedName;
                ImVec2 textSize = ImGui::CalcTextSize(truncatedName.c_str());
                if (textSize.x > maxNameWidth)
                {
                    truncatedName = truncatedName.substr(0, truncatedName.length() - 3) + xorstr_("...");
                }

                ImVec2 cursorPos = ImVec2(10.f, position);

                list->AddImage(reinterpret_cast<void*>(avatarTexture), ImVec2(cursorPos.x + 5.f, cursorPos.y + 5.f), ImVec2(cursorPos.x + avatarSize + 5.f, cursorPos.y + avatarSize + 5.f));

                ImVec2 namePosition = ImVec2(cursorPos.x + avatarSize + 10, cursorPos.y + (avatarSize / 2) - (textSize.y / 2));
                ImVec2 shadowOffset = ImVec2(1.f, 1.f);

                list->AddText(namePosition + shadowOffset, ImColor(0, 0, 0, 150), truncatedName.c_str());
                list->AddText(namePosition, ImColor(255, 255, 255, 255), truncatedName.c_str());

                position += entryHeight + 2.f;
            }
        }
    }
};