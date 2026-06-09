/**
 * @file main.cpp
 * @brief Entry point for the Antenna Radiation Pattern Visualization Tool.
 *
 * Application flow:
 *   SPLASH → MENU → MODE_2D / MODE_3D_SMOOTHED / MODE_3D_ROTATION → MENU
 *
 * Build:  see CMakeLists.txt
 * Author: AyaMrm
 */

#include "raylib.h"
#include "ui.h"
#include "parser.h"
#include "renderer2d.h"
#include "renderer3d.h"

// Window configuration
static constexpr int   SCREEN_W   = 1400;
static constexpr int   SCREEN_H   = 900;
static constexpr int   TARGET_FPS = 60;
static constexpr char  APP_TITLE[] = "Graphix - Antenna Radiation Viewer";

// main
int main() {
    InitWindow(SCREEN_W, SCREEN_H, APP_TITLE);
    SetTargetFPS(TARGET_FPS);
    SetExitKey(0);   // Disable automatic ESC-closes-window; we handle it manually

    // --- Load assets ----------------------------------------------------------
    Texture2D splash     = LoadTexture("assets/pic1.jpg");
    Texture2D background = LoadTexture("assets/background.jpg");

    if (splash.id == 0 || background.id == 0) {
        TraceLog(LOG_WARNING, "One or more textures could not be loaded – using fallback.");
    }

    // --- Application state ---------------------------------------------------
    ui::AppState    appState;
    ui::SplashAnim  splashAnim;

    parser::GraphData   graphData;
    renderer2d::State   state2d;
    renderer3d::State   state3d;

    Camera3D camera3d = {};
    camera3d.position   = { 15.0f, 10.0f, 15.0f };
    camera3d.target     = {  0.0f,  0.0f,  0.0f };
    camera3d.up         = {  0.0f,  1.0f,  0.0f };
    camera3d.fovy       = 45.0f;
    camera3d.projection = CAMERA_PERSPECTIVE;

    std::string lastLoadedFile;   // track which file is currently loaded

    // =========================================================================
    // Main loop
    // =========================================================================
    while (!WindowShouldClose()) {

        // --- Global ESC handling (return to menu from any render mode) -------
        if (IsKeyPressed(KEY_ESCAPE)) {
            switch (appState.currentScreen) {
                case ui::Screen::MODE_2D:
                case ui::Screen::MODE_3D_SMOOTHED:
                case ui::Screen::MODE_3D_ROTATION:
                    appState.currentScreen = ui::Screen::MENU;
                    state2d = {};           // reset 2D state
                    state3d = {};           // reset 3D state
                    break;
                case ui::Screen::ERROR:
                    appState.currentScreen = ui::Screen::MENU;
                    break;
                default:
                    break;
            }
        }

        // --- Lazy data load: (re)load only when file path changes or entering a mode ---
        if ((appState.currentScreen == ui::Screen::MODE_2D      ||
             appState.currentScreen == ui::Screen::MODE_3D_SMOOTHED ||
             appState.currentScreen == ui::Screen::MODE_3D_ROTATION) &&
             appState.filePath != lastLoadedFile)
        {
            graphData = parser::loadFile(appState.filePath);
            lastLoadedFile = appState.filePath;

            if (graphData.diagrams.empty()) {
                appState.errorMessage  = "Could not read data from: " + appState.filePath;
                appState.currentScreen = ui::Screen::ERROR;
            } else if (appState.currentScreen == ui::Screen::MODE_3D_SMOOTHED &&
                       graphData.diagrams.size() < 2) {
                appState.errorMessage  = "Smoothed 3D requires at least 2 diagrams (E-plane + H-plane).";
                appState.currentScreen = ui::Screen::ERROR;
            }
        }

        // --- Draw ------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        switch (appState.currentScreen) {
            case ui::Screen::SPLASH:
                ui::drawSplash(splash, splashAnim, appState, SCREEN_W, SCREEN_H);
                break;

            case ui::Screen::MENU:
                ui::drawMenu(background, appState, SCREEN_W, SCREEN_H);
                break;

            case ui::Screen::MODE_2D:
                ClearBackground(WHITE);
                renderer2d::drawFrame(graphData, state2d, SCREEN_W, SCREEN_H);
                break;

            case ui::Screen::MODE_3D_SMOOTHED:
                ClearBackground(BLACK);
                renderer3d::drawSmoothed(camera3d, graphData, SCREEN_W, SCREEN_H);
                break;

            case ui::Screen::MODE_3D_ROTATION:
                ClearBackground(BLACK);
                renderer3d::drawRotation(camera3d, graphData, state3d, SCREEN_W, SCREEN_H);
                break;

            case ui::Screen::ERROR:
                ui::drawError(appState, SCREEN_W, SCREEN_H);
                break;
        }

        EndDrawing();
    }

    // --- Cleanup -------------------------------------------------------------
    UnloadTexture(splash);
    UnloadTexture(background);
    CloseWindow();
    return 0;
}
