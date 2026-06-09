#pragma once

#include "parser.h"
#include "raylib.h"
#include <vector>

/**
 * @file renderer3d.h
 * @brief 3D spherical visualization of antenna radiation patterns.
 *
 * Two rendering modes:
 *  - SMOOTHED : Full spherical surface built from E-plane / H-plane interpolation,
 *               rendered as a triangle mesh with a heat-map color gradient.
 *  - ROTATION : Single diagram extruded by rotation around the Z-axis,
 *               drawn as stacked angular slices.
 *
 * Color mapping: blue (low intensity) → green → red (peak).
 */

namespace renderer3d {

/**
 * @brief Available 3D rendering modes.
 */
enum class Mode {
    SMOOTHED,   ///< Interpolated spherical surface (requires at least 2 diagrams)
    ROTATION    ///< Single-diagram rotation extrusion
};

/**
 * @brief Persistent state for the 3D view.
 */
struct State {
    bool  manualCamera = false;   ///< true = free camera, false = orbital auto-rotate
    int   diagramIndex = 0;       ///< Which diagram to use for ROTATION mode
};

/**
 * @brief Maps a scalar value in [minVal, maxVal] to an RGB heat-map color.
 *        Blue → Green → Red gradient.
 * @param value  Scalar to colorize
 * @param minVal Minimum of the data range
 * @param maxVal Maximum of the data range
 * @return Raylib Color
 */
Color heatmapColor(float value, float minVal, float maxVal);

/**
 * @brief Draws a single angular slice of the rotation diagram.
 *
 * @param data       Radiation data for this slice
 * @param axisOffset Offset along the rotation axis
 * @param axisType   0 = XY plane, 1 = XZ plane, 2 = YZ plane
 * @param minVal     Data minimum (for normalization)
 * @param maxVal     Data maximum (for normalization)
 */
void drawAngularSlice(const std::vector<double>& data,
                      float axisOffset, int axisType,
                      float minVal, float maxVal);

/**
 * @brief Renders the full 3D scene for SMOOTHED mode.
 *
 * Builds a spherical triangle mesh by interpolating E-plane and H-plane data
 * using cosine/sine weighting based on elevation angle theta.
 *
 * @param camera    Active 3D camera
 * @param data      Preprocessed graph data (needs >= 2 diagrams for best results)
 * @param screenW   Window width  (for colorbar placement)
 * @param screenH   Window height (for colorbar placement)
 */
void drawSmoothed(Camera3D& camera, const parser::GraphData& data, int screenW, int screenH);

/**
 * @brief Renders the full 3D scene for ROTATION mode.
 *
 * @param camera    Active 3D camera
 * @param data      Preprocessed graph data
 * @param state     Mutable state (camera mode toggled by SPACE)
 * @param screenW   Window width
 * @param screenH   Window height
 */
void drawRotation(Camera3D& camera, const parser::GraphData& data,
                  State& state, int screenW, int screenH);

/**
 * @brief Draws a vertical colorbar legend in the corner of the screen.
 *
 * @param minVal    Minimum data value (dB)
 * @param maxVal    Maximum data value (dB)
 * @param screenW   Window width
 * @param screenH   Window height
 */
void drawColorbar(float minVal, float maxVal, int screenW, int screenH);

} // namespace renderer3d
