#include "ui.h"
#include "parser.h"

#include "raygui.h"
#include "tinyfiledialogs.h"

#include <raymath.h>
#include <cstring>
#include <string>
#include <algorithm>

namespace ui {

void drawSplash(Texture2D texture, SplashAnim& a, AppState& state, int screenW, int screenH) {
    // Draw background
    DrawTexturePro(texture,
                   { 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) },
                   { 0, 0, static_cast<float>(screenW),        static_cast<float>(screenH) },
                   { 0, 0 }, 0.0f, WHITE);

    const char* title       = "GRAPHIX";
    const int   titleSize   = 100;
    const char* subtitle    = "Click to start";
    const int   subtitleSize = 22;

    const float targetTitleX  = (screenW - static_cast<float>(MeasureText(title, titleSize))) * 0.5f;
    const float targetSubX    = (screenW - static_cast<float>(MeasureText(subtitle, subtitleSize))) * 0.5f;
    const int   titleY        = screenH / 2 - 120;

    // Animate slide-in
    a.titleX     += (targetTitleX - a.titleX) * 0.05f;
    a.titleAlpha  = std::min(a.titleAlpha + 0.02f, 1.0f);

    if (a.titleX >= targetTitleX - 15.0f) {
        a.buttonX    += (targetSubX - a.buttonX) * 0.05f;
        a.buttonAlpha = std::min(a.buttonAlpha + 0.02f, 1.0f);
    }

    // Neon glow pulse
    a.glowPhase += 0.05f;
    float glow = (sinf(a.glowPhase) + 1.0f) * 0.5f;
    Color titleColor = ColorFromHSV(200 + 50 * sinf(a.glowPhase * 2), 0.9f, 0.8f + 0.2f * glow);

    // Drop shadow
    DrawText(title, static_cast<int>(a.titleX) + 3, titleY + 3, titleSize,
             Fade(BLACK, a.titleAlpha * 0.35f));
    // Title
    DrawText(title, static_cast<int>(a.titleX), titleY, titleSize, Fade(titleColor, a.titleAlpha));

    // Underline laser effect
    if (glow > 0.65f) {
        int lineW = static_cast<int>(MeasureText(title, titleSize) * glow);
        DrawRectangle(static_cast<int>(a.titleX), titleY + titleSize - 4,
                      lineW, 3, Fade(SKYBLUE, 0.35f * glow));
    }

    // Subtitle / CTA
    Vector2 mouse = GetMousePosition();
    Rectangle btnRect = { a.buttonX, static_cast<float>(titleY + titleSize + 35),
                          static_cast<float>(MeasureText(subtitle, subtitleSize)),
                          static_cast<float>(subtitleSize) };
    bool hover = CheckCollisionPointRec(mouse, btnRect);

    Color subColor = hover ? SKYBLUE : WHITE;
    DrawText(subtitle, static_cast<int>(a.buttonX),
             titleY + titleSize + 35, subtitleSize, Fade(subColor, a.buttonAlpha));

    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        state.currentScreen = Screen::MENU;
}

void drawMenu(Texture2D background, AppState& state, int screenW, int screenH) {
    DrawTexturePro(background,
                   { 0, 0, static_cast<float>(background.width), static_cast<float>(background.height) },
                   { 0, 0, static_cast<float>(screenW), static_cast<float>(screenH) },
                   { 0, 0 }, 0.0f, WHITE);

    // Title
    DrawText("Polar & Spherical Radiation Viewer", 20, 20, 26, WHITE);
    DrawText("Antenna Radiation Pattern Visualization Tool", 20, 52, 16,
             Color{ 180, 220, 255, 220 });

    // File path input
    static char pathBuf[512] = "";
    if (!state.filePath.empty() && pathBuf[0] == '\0')
        strncpy(pathBuf, state.filePath.c_str(), sizeof(pathBuf) - 1);

    const float boxW = 520.0f, boxH = 40.0f;
    const float boxX = (screenW - boxW) * 0.5f;
    const float boxY = screenH * 0.5f - 60.0f;

    DrawText("Data file (.atn)", static_cast<int>(boxX), static_cast<int>(boxY - 28), 18, WHITE);

    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL,  0xC8C8C8FF);
    GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, 0xDDDDDDFF);
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL,  0x000000FF);
    GuiTextBox({ boxX, boxY, boxW, boxH }, pathBuf, sizeof(pathBuf), true);

    // Browse button
    if (GuiButton({ boxX + boxW + 12, boxY, 100, boxH }, "Browse...")) {
        const char* filters[] = { "*.atn", "*.txt" };
        const char* picked = tinyfd_openFileDialog(
            "Select measurement file", "", 2, filters, "Antenna data (*.atn, *.txt)", 0);
        if (picked) {
            strncpy(pathBuf, picked, sizeof(pathBuf) - 1);
            state.filePath = picked;
        }
    }

    // Sync text box → state
    state.filePath = pathBuf;

    // Mode buttons
    const float btnW = 220.0f, btnH = 38.0f;
    const float btnX = (screenW - btnW) * 0.5f;
    const float btnY = boxY + 90.0f;
    const float gap  = 16.0f;

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x2255AAFF);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, 0xFFFFFFFF);

    auto tryLoad = [&]() -> bool {
        if (state.filePath.empty()) {
            state.errorMessage = "Please select a data file first.";
            state.currentScreen = Screen::ERROR;
            return false;
        }
        return true;
    };

    if (GuiButton({ btnX, btnY, btnW, btnH }, "2D Polar Plot")) {
        if (tryLoad()) state.currentScreen = Screen::MODE_2D;
    }

    if (GuiButton({ btnX, btnY + btnH + gap, btnW, btnH }, "3D Smoothed Surface")) {
        if (tryLoad()) {
            // Will be validated in main
            state.currentScreen = Screen::MODE_3D_SMOOTHED;
        }
    }

    if (GuiButton({ btnX, btnY + 2 * (btnH + gap), btnW, btnH }, "3D Rotation Extrusion")) {
        if (tryLoad()) state.currentScreen = Screen::MODE_3D_ROTATION;
    }

    // Footer
    DrawText("v1.0  |  C++ / Raylib  |  Antenna Radiation Pattern Viewer",
             10, screenH - 24, 14, Color{ 180, 180, 180, 180 });
}

void drawError(AppState& state, int screenW, int screenH) {
    DrawRectangle(0, 0, screenW, screenH, { 20, 20, 30, 255 });

    const char* title = "Error";
    DrawText(title,
             (screenW - MeasureText(title, 40)) / 2,
             screenH / 2 - 80, 40, RED);

    DrawText(state.errorMessage.c_str(),
             (screenW - MeasureText(state.errorMessage.c_str(), 20)) / 2,
             screenH / 2 - 20, 20, LIGHTGRAY);

    if (GuiButton({ static_cast<float>(screenW) / 2 - 70,
                    static_cast<float>(screenH) / 2 + 40, 140, 38 }, "Back to Menu")) {
        state.currentScreen = Screen::MENU;
        state.errorMessage.clear();
    }
}

} // namespace ui