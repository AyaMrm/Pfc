#pragma once

#include "parser.h"
#include "raylib.h"
#include <vector>

/**
 * @file renderer2d.h
 * @brief 2D polar plot rendering of antenna radiation diagrams.
 *
 * Draws interactive polar graphs with:
 *  - Concentric reference circles with dB labels
 *  - Angular degree markers every 45 degrees
 *  - Up to two overlaid radiation diagrams (E-plane / H-plane)
 *  - Half-power beamwidth (HPBW) markers at the -3 dB points
 */

namespace renderer2d {

/**
 * @brief Persistent UI state for the 2D view.
 */
struct State {
    bool showSecondDiagram = false;    ///< Whether the second overlay diagram is visible
};

/**
 * @brief Computes the half-power beamwidth (-3 dB opening angle).
 *
 * Searches left and right from the peak for the first crossing of peak - 3 dB,
 * using linear interpolation for sub-index precision.
 *
 * @param diagram Normalized radiation diagram (peak = 0 dB)
 * @return Vector of three values: {leftIndex, rightIndex, peakValue}
 *         Returns {0, 0, 0} if no valid crossing is found.
 */
std::vector<double> computeHPBW(const std::vector<double>& diagram);

/**
 * @brief Renders one complete frame of the 2D polar view.
 *
 * Call this inside BeginDrawing() / EndDrawing().
 *
 * @param data       Preprocessed graph data from the parser
 * @param state      Mutable UI state (button interactions update this)
 * @param screenW    Window width  in pixels
 * @param screenH    Window height in pixels
 */
void drawFrame(const parser::GraphData& data, State& state, int screenW, int screenH);

} // namespace renderer2d
