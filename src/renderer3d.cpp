#include "renderer3d.h"

#include <rlgl.h>
#include <raymath.h>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

namespace renderer3d {

static constexpr float MAX_SPHERE_RADIUS = 5.0f;
static constexpr int   THETA_STEPS       = 180;
static constexpr int   PHI_STEPS         = 180;
static constexpr int   SLICE_STEPS       = 120;

Color heatmapColor(float value, float minVal, float maxVal) {
    float range = maxVal - minVal;
    if (range == 0.0f) range = 1.0f;

    float t = Clamp((value - minVal) / range, 0.0f, 1.0f);

    unsigned char r, g, b;
    if (t < 0.5f) {
        float lt = t / 0.5f;
        r = 0;
        g = static_cast<unsigned char>(255 * lt);
        b = static_cast<unsigned char>(255 * (1.0f - lt));
    } else {
        float lt = (t - 0.5f) / 0.5f;
        r = static_cast<unsigned char>(255 * lt);
        g = static_cast<unsigned char>(255 * (1.0f - lt));
        b = 0;
    }
    return Color{ r, g, b, 255 };
}

void drawColorbar(float minVal, float maxVal, int screenW, int screenH) {
    const int barW   = 20;
    const int barH   = 200;
    const int barX   = screenW - 60;
    const int barY   = 50;
    const int steps  = 100;
    const int labels = 5;

    // Semi-transparent background
    DrawRectangle(barX - 10, barY - 10, barW + 70, barH + 60, { 0, 0, 0, 120 });

    // Color segments
    for (int i = 0; i < steps; ++i) {
        float t     = static_cast<float>(i) / (steps - 1);
        float value = minVal + t * (maxVal - minVal);
        Color col   = heatmapColor(value, minVal, maxVal);
        int   y     = barY + (steps - 1 - i) * (barH / steps);
        DrawRectangle(barX, y, barW, barH / steps + 1, col);
    }

    // Labels
    for (int i = 0; i < labels; ++i) {
        float value = minVal + static_cast<float>(i) / (labels - 1) * (maxVal - minVal);
        int   y     = barY + (labels - 1 - i) * (barH / (labels - 1));
        DrawText(TextFormat("%.0f dB", value), barX + barW + 5, y - 5, 10, WHITE);
    }
}

// Angular slice (ROTATION mode)
void drawAngularSlice(const std::vector<double>& data,
                      float axisOffset, int axisType,
                      float minVal, float maxVal)
{
    if (data.empty()) return;

    const float angleStep = 360.0f / SLICE_STEPS;

    for (int i = 0; i < SLICE_STEPS; ++i) {
        float theta1 = static_cast<float>(i)     * angleStep * DEG2RAD;
        float theta2 = static_cast<float>(i + 1) * angleStep * DEG2RAD;

        int idx1 = (i       * static_cast<int>(data.size()) / SLICE_STEPS) % static_cast<int>(data.size());
        int idx2 = ((i + 1) * static_cast<int>(data.size()) / SLICE_STEPS) % static_cast<int>(data.size());

        float r1 = MAX_SPHERE_RADIUS * Clamp(
            (static_cast<float>(data[idx1]) - minVal) / (maxVal - minVal), 0.0f, 1.0f);
        float r2 = MAX_SPHERE_RADIUS * Clamp(
            (static_cast<float>(data[idx2]) - minVal) / (maxVal - minVal), 0.0f, 1.0f);

        Vector3 center{}, p1{}, p2{};
        switch (axisType) {
            case 0: center = {0,0,axisOffset}; p1={r1*cosf(theta1),r1*sinf(theta1),0}; p2={r2*cosf(theta2),r2*sinf(theta2),0}; break;
            case 1: center = {0,axisOffset,0}; p1={r1*cosf(theta1),0,r1*sinf(theta1)}; p2={r2*cosf(theta2),0,r2*sinf(theta2)}; break;
            case 2: center = {axisOffset,0,0}; p1={0,r1*cosf(theta1),r1*sinf(theta1)}; p2={0,r2*cosf(theta2),r2*sinf(theta2)}; break;
        }

        Color col1 = heatmapColor(static_cast<float>(data[idx1]), minVal, maxVal);
        Color col2 = heatmapColor(static_cast<float>(data[idx2]), minVal, maxVal);
        DrawTriangle3D(center, p1, p2, col1);
        DrawLine3D(p1, p2, col2);
    }
}

// ROTATION mode
void drawRotation(Camera3D& camera, const parser::GraphData& data,
                  State& state, int screenW, int screenH)
{
    if (IsKeyPressed(KEY_SPACE)) state.manualCamera = !state.manualCamera;

    UpdateCamera(&camera, state.manualCamera ? CAMERA_FREE : CAMERA_ORBITAL);

    if (data.diagrams.empty()) return;
    const auto& diag = data.diagrams[state.diagramIndex];
    if (diag.empty()) return;

    double minVal = *std::min_element(diag.begin(), diag.end());
    double maxVal = *std::max_element(diag.begin(), diag.end());

    BeginMode3D(camera);
    DrawGrid(20, 1.0f);
    DrawLine3D({0,0,0}, {12,0,0}, RED);
    DrawLine3D({0,0,0}, {0,12,0}, GREEN);
    DrawLine3D({0,0,0}, {0,0,12}, BLUE);

    for (int i = 0; i < 180; ++i) {
        rlPushMatrix();
        rlRotatef(static_cast<float>(i) * 2.0f, 1, 0, 0);
        drawAngularSlice(diag, 0, 0, static_cast<float>(minVal), static_cast<float>(maxVal));
        drawAngularSlice(diag, 0, 1, static_cast<float>(minVal), static_cast<float>(maxVal));
        rlPopMatrix();
    }

    DrawSphereWires({0,0,0}, MAX_SPHERE_RADIUS, 32, 32, { 200,200,200,80 });
    EndMode3D();

    drawColorbar(static_cast<float>(minVal), static_cast<float>(maxVal), screenW, screenH);

    DrawText("3D Rotation Mode", 10, 10, 20, DARKGRAY);
    DrawText(state.manualCamera
             ? "Free camera - WASD / mouse drag / scroll to zoom  |  T=top  B=bottom"
             : "Orbital camera - SPACE to switch to free camera",
             10, screenH - 30, 16, WHITE);
}

// SMOOTHED mode
void drawSmoothed(Camera3D& camera, const parser::GraphData& data, int screenW, int screenH) {
    if (data.diagrams.empty()) return;

    const auto& planE = data.diagrams[0];
    const auto& planH = (data.diagrams.size() > 1) ? data.diagrams[1] : data.diagrams[0];

    if (planE.empty()) return;

    // Compute global range
    double globalMin = planE[0], globalMax = planE[0];
    for (const auto& diag : { planE, planH }) {
        for (double v : diag) {
            if (std::isfinite(v)) {
                globalMin = std::min(globalMin, v);
                globalMax = std::max(globalMax, v);
            }
        }
    }
    double range = globalMax - globalMin;
    if (range == 0.0) range = 1.0;

    const size_t NE = planE.size();
    const size_t NH = planH.size();

    // Build sphere mesh
    std::vector<std::vector<Vector3>> pts(THETA_STEPS, std::vector<Vector3>(PHI_STEPS));
    std::vector<std::vector<Color>>   cols(THETA_STEPS, std::vector<Color>(PHI_STEPS));

    for (int t = 0; t < THETA_STEPS; ++t) {
        float theta = static_cast<float>(t) * (PI / (THETA_STEPS - 1));

        for (int p = 0; p < PHI_STEPS; ++p) {
            float phi = static_cast<float>(p) * (2.0f * PI / PHI_STEPS);

            // Map to E-plane index
            float tIdxF = static_cast<float>(t) * 180.0f / (THETA_STEPS - 1);
            if (tIdxF > 180.0f) tIdxF = 360.0f - tIdxF;
            tIdxF = (tIdxF / 180.0f) * static_cast<float>(NE - 1);

            // Map to H-plane index
            float pIdxF = static_cast<float>(p) * static_cast<float>(NH - 1) / (PHI_STEPS - 1);

            // Linear interpolation – E plane
            int iE0 = std::min(static_cast<int>(tIdxF), static_cast<int>(NE - 1));
            int iE1 = std::min(iE0 + 1, static_cast<int>(NE - 1));
            float fracE = tIdxF - iE0;
            double valE = planE[iE0] + fracE * (planE[iE1] - planE[iE0]);

            // Linear interpolation – H plane
            int iH0 = std::min(static_cast<int>(pIdxF), static_cast<int>(NH - 1));
            int iH1 = std::min(iH0 + 1, static_cast<int>(NH - 1));
            float fracH = pIdxF - iH0;
            double valH = planH[iH0] + fracH * (planH[iH1] - planH[iH0]);

            // Weighted combination: E-plane dominates at poles, H-plane at equator
            float wE = fabsf(cosf(theta));
            float wH = sinf(theta);
            float wT = wE + wH;
            if (wT == 0.0f) wT = 1.0f;
            double dataVal = (valE * wE + valH * wH) / wT;

            // Smoothing: taper toward poles using sin(theta)
            double eff = dataVal * sinf(theta);

            float r = MAX_SPHERE_RADIUS * Clamp(
                static_cast<float>((eff - globalMin) / range), 0.0f, 1.0f);

            pts[t][p]  = { r * sinf(theta) * cosf(phi),
                           r * cosf(theta),
                           r * sinf(theta) * sinf(phi) };
            cols[t][p] = heatmapColor(static_cast<float>(eff),
                                      static_cast<float>(globalMin),
                                      static_cast<float>(globalMax));
        }
    }

    UpdateCamera(&camera, CAMERA_ORBITAL);

    BeginMode3D(camera);

    DrawSphereWires({0,0,0}, MAX_SPHERE_RADIUS, 32, 32, { 200,200,200,60 });
    DrawLine3D({0,0,0}, {12,0,0}, RED);
    DrawLine3D({0,0,0}, {0,12,0}, GREEN);
    DrawLine3D({0,0,0}, {0,0,12}, BLUE);

    // Triangle mesh
    for (int t = 0; t < THETA_STEPS - 1; ++t) {
        for (int p = 0; p < PHI_STEPS; ++p) {
            int np = (p + 1) % PHI_STEPS;
            DrawTriangle3D(pts[t][p], pts[t+1][p], pts[t+1][np], cols[t][p]);
            DrawTriangle3D(pts[t][p], pts[t+1][np], pts[t][np],   cols[t][p]);
        }
    }

    // Pole labels
    Vector2 northScr = GetWorldToScreen({0, MAX_SPHERE_RADIUS + 0.8f, 0}, camera);
    Vector2 southScr = GetWorldToScreen({0, -(MAX_SPHERE_RADIUS + 0.8f), 0}, camera);
    EndMode3D();

    DrawText("N", static_cast<int>(northScr.x), static_cast<int>(northScr.y), 20, BLACK);
    DrawText("S", static_cast<int>(southScr.x), static_cast<int>(southScr.y), 20, BLACK);

    drawColorbar(static_cast<float>(globalMin), static_cast<float>(globalMax), screenW, screenH);

    DrawText("3D Smoothed Spherical Mode", 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Power range: %.1f dB to %.1f dB",
                        static_cast<float>(globalMin),
                        static_cast<float>(globalMax)), 10, 35, 16, DARKGRAY);
    DrawText("ESC : Back to menu", 10, screenH - 28, 16, GRAY);
}

} // namespace renderer3d
