#pragma once

#include "raylib.h"
#include <string>

/**
 * @file ui.h
 * @brief Application state machine and UI screens.
 *
 * Manages the top-level navigation between:
 *   SPLASH     → Welcome animation screen
 *   MENU       → File picker + mode selection
 *   MODE_2D    → 2D polar plot
 *   MODE_3D_S  → 3D smoothed spherical surface
 *   MODE_3D_R  → 3D rotation extrusion
 *   ERROR      → Error screen with message
 */

namespace ui {

/**
 * @brief Top-level application screens.
 */
enum class Screen {
    SPLASH,
    MENU,
    MODE_2D,
    MODE_3D_SMOOTHED,
    MODE_3D_ROTATION,
    ERROR
};

/**
 * @brief Shared application state passed between screens.
 */
struct AppState {
    Screen      currentScreen  = Screen::SPLASH;
    std::string filePath;          ///< Currently loaded file path
    std::string errorMessage;      ///< Error text shown on the ERROR screen
};

/**
 * @brief Persistent animation state for the splash screen.
 */
struct SplashAnim {
    float titleX     = -600.0f;
    float buttonX    = -600.0f;
    float titleAlpha = 0.0f;
    float buttonAlpha= 0.0f;
    float glowPhase  = 0.0f;
};

/**
 * @brief Renders the animated splash / welcome screen.
 *        Sets state.currentScreen = MENU when the user clicks.
 *
 * @param texture   Background texture
 * @param anim      Mutable animation state (persists across frames)
 * @param state     Mutable app state
 * @param screenW   Window width
 * @param screenH   Window height
 */
void drawSplash(Texture2D texture, SplashAnim& anim, AppState& state, int screenW, int screenH);

/**
 * @brief Renders the main menu (file picker + mode buttons).
 *        Updates state.currentScreen and state.filePath on user action.
 *
 * @param background  Background texture
 * @param state       Mutable app state
 * @param screenW     Window width
 * @param screenH     Window height
 */
void drawMenu(Texture2D background, AppState& state, int screenW, int screenH);

/**
 * @brief Renders a full-screen error message with a back-to-menu button.
 *
 * @param state   Mutable app state (reads errorMessage, can reset screen)
 * @param screenW Window width
 * @param screenH Window height
 */
void drawError(AppState& state, int screenW, int screenH);

} // namespace ui
