# 🦈 Luftwaffe Visuals  
Free collection of CS2 (Counter-Strike 2) ImGui-based visual features / indicators

A small set of clean, modern-looking **ESP-style overlays** and HUD elements written in C++ with ImGui + DirectX 11.  
Released for **educational purposes**, reverse-engineering learning, and fun.  

**These are client-side visual indicators only — no aimbot, triggerbot, wallhack, or memory writing code is included.**

## ✨ Included Visuals

| Feature              | Description                                                                 |
|----------------------|-----------------------------------------------------------------------------|
| **Keybind List**     | Shows currently active aimbot / triggerbot / magnet keys (right side)       |
| **Bomb Indicator**   | Circular timer + defuse info                                                | 
| **Velocity**         | Current 2D speed (units/s) in center-bottom                                 |
| **Stamina**          | Remaining stamina bar/value (center-bottom)                                 |
| **Spectator List**   | Left-side list of players spectating you + Steam avatars                    |
| **Watermark**        | Simple branded top-left watermark with accent color                         |
| **Keystrokes**       | WASD + SPACE HUD with activation glow (center-bottom)                       |

All features use:
- ImGui draw lists
- Custom accent colors from settings
- Shadow text for readability
- `xorstr_` string encryption
- Steam avatar loading (spectator list)

## ⚙️ Requirements

- Counter-Strike 2 (latest version)
- ImGui (docking branch recommended)
- DirectX 11 rendering backend
- CS2 SDK / interface definitions (`CCSPlayerController`, `CCSPlayerPawn`, etc.)
- External dependencies: `fmt`, `xorstr`, ImGui notify (optional)

```cpp
// Example in your render hook
ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

features::keybind_list::handle(draw_list);
features::bomb_indicator::handle(draw_list);
features::velocity::handle(draw_list);
features::stamina::handle(draw_list);
features::spectator_list::handle(device, context);  // needs D3D11 device & context
features::watermark::handle(draw_list);
features::key_strokes::handle(draw_list);
