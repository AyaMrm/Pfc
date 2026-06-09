#include "renderer2d.h"

#include "raygui.h"
#include <raymath.h>

#include <algorithm>
#include <cmath>
#include <string>

namespace renderer2d {

static constexpr int   MAX_RADIUS_PX = 300;
static constexpr int   CIRCLE_STEPS  = 7;
static constexpr float LINE_THICK    = 2.0f;
static const     Color COL_PLANE1    = BLUE;
static const     Color COL_PLANE2    = ORANGE;
static const     Color COL_HPBW1    = { 168, 241, 255, 255 };
static const     Color COL_HPBW2    = { 242, 192, 120, 255 };

// HPBW computation
std::vector<double> computeHPBW(const std::vector<double>& diagram) {
    std::vector<double> result(3, 0.0);
    if (diagram.empty()) return result;

    // Peak is at index 0 after normalization + rotation
    const double peakVal  = diagram[0];
    const double threshold = peakVal - 3.0;   // -3 dB point
    const size_t N = diagram.size();

    double leftIdx  = 0.0;
    double rightIdx = 0.0;

    // Search left (going backwards from 0 to the end of the vector)
    for (size_t i = 1; i < N; ++i) {
        if (diagram[i] <= threshold) {
            double frac = (threshold - diagram[i]) / (diagram[i - 1] - diagram[i]);
            leftIdx = static_cast<double>(i) - frac;
            break;
        }
    }

    // Search right (going forward from 0)
    for (size_t i = N - 1; i > 0; --i) {
        if (diagram[i] <= threshold) {
            double frac = (threshold - diagram[i]) / (diagram[i + 1 < N ? i + 1 : 0] - diagram[i]);
            rightIdx = static_cast<double>(i) + frac;
            break;
        }
    }

    result[0] = leftIdx;
    result[1] = rightIdx;
    result[2] = peakVal;
    return result;
}

// Internal helpers
static void drawDiagram(const std::vector<double>& diag,
                        Vector2 center, float maxR,
                        float minDb, float maxDb,
                        Color color)
{
    if (diag.empty()) return;

    const size_t N = diag.size();
    const double angleStep = 360.0 / static_cast<double>(N);
    std::vector<Vector2> pts(N);

    for (size_t i = 0; i < N; ++i) {
        float thetaRad = static_cast<float>(i * angleStep) * DEG2RAD;
        float norm     = Clamp((static_cast<float>(diag[i]) - minDb) / (maxDb - minDb), 0.0f, 1.0f);
        float r        = maxR * norm;
        pts[i] = { center.x + r * cosf(thetaRad),
                   center.y + r * sinf(thetaRad) };
    }

    for (size_t i = 1; i < N; ++i)
        DrawLineEx(pts[i - 1], pts[i], LINE_THICK, color);
    DrawLineEx(pts.back(), pts.front(), LINE_THICK, color);
}

static void drawHPBWMarkers(const std::vector<double>& diag,
                            Vector2 center, float maxR,
                            float minDb, float maxDb,
                            Color markerColor)
{
    std::vector<double> hpbw = computeHPBW(diag);
    if (hpbw[0] == 0.0 && hpbw[1] == 0.0) return;

    const double angleStep = 360.0 / static_cast<double>(diag.size());
    const double threshold = hpbw[2] - 3.0;
    float norm = Clamp((static_cast<float>(threshold) - minDb) / (maxDb - minDb), 0.0f, 1.0f);
    float r    = maxR * norm;

    float thetaL = static_cast<float>(hpbw[0] * angleStep) * DEG2RAD;
    float thetaR = static_cast<float>(hpbw[1] * angleStep) * DEG2RAD;

    Vector2 pL = { center.x + r * cosf(thetaL), center.y + r * sinf(thetaL) };
    Vector2 pR = { center.x + r * cosf(thetaR), center.y + r * sinf(thetaR) };

    DrawCircleV(pL, 5.0f, markerColor);
    DrawCircleV(pR, 5.0f, markerColor);
    DrawLineEx(pL, center, LINE_THICK, markerColor);
    DrawLineEx(pR, center, LINE_THICK, markerColor);

    double beamwidth = std::abs(hpbw[0] - hpbw[1]) * angleStep;
    beamwidth = std::min(beamwidth, 360.0 - beamwidth);

    std::string label = "HPBW: " + std::to_string(static_cast<int>(beamwidth)) + " deg";
    DrawText(label.c_str(), 10, (markerColor.r == COL_HPBW1.r) ? 40 : 120, 18, DARKGRAY);
}

// Public API
void drawFrame(const parser::GraphData& data, State& state, int screenW, int screenH) {
    if (data.diagrams.empty()) {
        DrawText("Error: No data loaded.", 20, 20, 20, RED);
        return;
    }

    const Vector2 center    = { screenW * 0.5f, screenH * 0.5f };
    const float   maxRadius = static_cast<float>(MAX_RADIUS_PX);

    // --- Compute per-diagram ranges from raw (pre-rotation) characteristics ---
    auto getRange = [&](int idx) -> std::pair<float, float> {
        if (idx >= (int)data.characteristics.size()) return { -60.f, 0.f };
        const auto& ch = data.characteristics[idx];
        float maxDb = static_cast<float>(ch[1]) - static_cast<float>(ch[1]); // after normalize, max = 0
        float minDb = static_cast<float>(ch[2]) - static_cast<float>(ch[1]); // min = original_min - original_max
        if (maxDb == minDb) maxDb = minDb + 1.f;
        return { minDb, maxDb };
    };

    auto [min1, max1] = getRange(0);
    auto [min2, max2] = getRange(1);

    // --- Reference circles ---
    for (int i = 0; i < CIRCLE_STEPS; ++i) {
        float r  = maxRadius * static_cast<float>(i) / (CIRCLE_STEPS - 1);
        float db = state.showSecondDiagram
                   ? min2 + i * (max2 - min2) / (CIRCLE_STEPS - 1)
                   : min1 + i * (max1 - min1) / (CIRCLE_STEPS - 1);
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y),
                        r, LIGHTGRAY);
        Color lc = state.showSecondDiagram ? Color{255,204,153,255} : GRAY;
        DrawText(TextFormat("%.0f", db), static_cast<int>(center.x + r + 6),
                 static_cast<int>(center.y), 10, lc);
    }

    // --- Angular labels every 45° ---
    for (int angle = 0; angle < 360; angle += 45) {
        float rad = static_cast<float>(angle) * DEG2RAD;
        float x   = center.x + (maxRadius + 22) * cosf(rad);
        float y   = center.y + (maxRadius + 22) * sinf(rad);
        DrawText(TextFormat("%d deg", angle),
                 static_cast<int>(x) - (angle == 180 ? 20 : 0),
                 static_cast<int>(y) - 10, 10, GRAY);
    }

    // --- Diagram 1 (always visible) ---
    drawDiagram(data.diagrams[0], center, maxRadius, min1, max1, COL_PLANE1);
    drawHPBWMarkers(data.diagrams[0], center, maxRadius, min1, max1, COL_HPBW1);

    DrawText(TextFormat("E-plane  [%.0f dB .. 0 dB]", min1), 10, 10, 18, COL_PLANE1);

    // --- Diagram 2 (toggle) ---
    if (state.showSecondDiagram && data.diagrams.size() >= 2) {
        drawDiagram(data.diagrams[1], center, maxRadius, min2, max2, COL_PLANE2);
        drawHPBWMarkers(data.diagrams[1], center, maxRadius, min2, max2, COL_HPBW2);
        DrawText(TextFormat("H-plane  [%.0f dB .. 0 dB]", min2), 10, 90, 18, COL_PLANE2);
    }

    // --- Buttons ---
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0xC8C8C8FF);

    if (data.diagrams.size() >= 2) {
        const char* btnLabel = state.showSecondDiagram ? "Hide H-plane" : "Show H-plane";
        if (GuiButton({ static_cast<float>(screenW) - 160, static_cast<float>(screenH) - 50, 140, 30 }, btnLabel))
            state.showSecondDiagram = !state.showSecondDiagram;
    }

    DrawText("ESC : Back to menu", 10, screenH - 28, 16, GRAY);
}

} // namespace renderer2d
