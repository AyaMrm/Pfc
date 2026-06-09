# Graphix – Antenna Radiation Pattern Viewer

> **Interactive 2D/3D visualization of antenna radiation diagrams from `.atn` measurement files.**  
> Built in C++17 with [Raylib](https://www.raylib.com/) and RayGUI.

---

## Table of Contents

1. [Overview](#overview)
2. [Background – What is an Antenna Radiation Pattern?](#background)
3. [Project Objectives](#project-objectives)
4. [Features](#features)
5. [Data File Format](#data-file-format)
6. [Data Processing Pipeline](#data-processing-pipeline)
7. [Visualization Modes](#visualization-modes)
8. [Technical Highlights](#technical-highlights)
9. [Project Structure](#project-structure)
10. [Building](#building)
11. [Usage](#usage)
12. [Results and Limitations](#results-and-limitations)
13. [Future Improvements](#future-improvements)
14. [References](#references)
15. [License](#license)
16. [Author](#author)

---

## Overview

**Graphix** is a desktop application that parses raw antenna measurement data and renders it as interactive polar and spherical plots.

It was developed as a **pluridisciplinary academic project** during the **2024–2025 academic year**, under the supervision of **Dr. Lahmissi**.

The tool targets engineers and researchers working in RF, telecommunications, and antenna design who need a **lightweight, dependency-light alternative** to MATLAB or specialized software for visualizing radiation patterns.

---

## Background – What is an Antenna Radiation Pattern?

A **radiation pattern** describes how an antenna distributes transmitted power in space. Understanding this distribution is essential in antenna design, telecommunications, and radar systems.

### Why polar and spherical coordinates?

Polar and spherical coordinate systems are the natural choice for representing phenomena with **angular, circular, or spatial characteristics**. Compared to Cartesian coordinates, they offer a **clearer and more intuitive representation** of directional data.

They are widely used in:
- Antenna theory and telecommunications
- Radar systems
- Astronomy and physics
- Wave propagation analysis

### Key concepts

| Term | Definition |
|---|---|
| **dB (decibel)** | Logarithmic power ratio; allows large dynamic ranges to be displayed compactly |
| **dBm** | Power relative to 1 mW; used in RF link budget analysis |
| **HPBW** | Half-Power BeamWidth – the angular width of the main lobe at the −3 dB points; a key antenna specification |
| **E-plane** | Principal plane of a linearly polarized antenna containing the electric field vector |
| **H-plane** | Principal plane containing the magnetic field vector; used with E-plane to reconstruct a full 3D pattern |
| **Main lobe** | Direction of maximum radiation |
| **Side lobes** | Secondary radiation directions, usually unwanted |

---

## Project Objectives

The software aims to:

- Import measurement data from `.atn` and `.txt` files
- Parse, clean, and normalize raw data automatically
- Display data as interactive **2D polar graphs** and **3D spherical visualizations**
- Allow user interaction: rotation, zooming, and mode switching
- Facilitate the interpretation of antenna performance without requiring MATLAB or proprietary tools

---

## Features

| Feature | Details |
|---|---|
| **File parsing** | Reads `.atn` and `.txt` files, strips metadata (dates, timestamps, labels), extracts numeric data into individual radiation diagrams |
| **2D Polar Plot** | Renders up to two overlaid diagrams (E-plane / H-plane) with dB reference circles, angular labels, and automatic HPBW (−3 dB) markers |
| **3D Smoothed Surface** | Builds a full spherical triangle mesh by interpolating E-plane and H-plane data with cosine/sine elevation weighting |
| **3D Rotation Mode** | Extrudes a single diagram by rotation around the Z-axis as stacked angular slices |
| **Heat-map coloring** | Blue → Green → Red gradient mapped to signal intensity across all 3D views |
| **Interactive camera** | Orbital (auto-rotate) and free camera modes (WASD + mouse drag) in 3D views |
| **Data normalization** | Peak set to 0 dB, diagram rotated so the main lobe is at 0° – consistent across all modes |

---

## Data File Format

The parser accepts `.atn` files structured as alternating blocks of metadata and numeric measurements:

```
15-03-2024          ← date (skipped)
08:30:00            ← time (skipped)
Antenna_XY          ← label (skipped)
-12.5               ← measurement value (kept)
-10.3
-8.7
...
15-03-2024          ← next block separator (starts a new diagram)
...
```

Each numeric block separated by a metadata header becomes **one radiation diagram**. A file with two such blocks produces an E-plane and H-plane diagram, enabling full 3D reconstruction.

---

## Data Processing Pipeline

The raw data goes through the following steps before rendering:

1. **File reading** – Open and parse the file line by line
2. **Format validation** – Detect and skip non-numeric lines (dates, labels, timestamps)
3. **Segmentation** – Split the data stream into individual radiation pattern arrays
4. **Normalization** – Subtract the maximum value so the peak becomes 0 dB
5. **Reordering** – Rotate the array so the main lobe aligns at 0°
6. **Rendering preparation** – Convert normalized dB values to polar/spherical coordinates for display

This pipeline ensures consistent, interpretable output regardless of the raw file's formatting or measurement offset.

---

## Visualization Modes

### 2D Polar Plot

- Data plotted in polar coordinates
- **Radius** represents signal intensity (in dB)
- **Angle** represents direction (0°–360°)
- Concentric reference circles indicate intensity levels
- HPBW markers automatically placed at the −3 dB points
- Supports overlaying two diagrams (E-plane + H-plane) simultaneously

### 3D Smoothed Surface

- Requires at least **two diagrams** (E-plane + H-plane)
- Builds a complete spherical mesh by interpolating both planes using cosine/sine elevation weighting
- Intensity mapped to both **surface distance from origin** and **color** (heat-map)
- Full orbital and free camera navigation

### 3D Rotation Mode

- Works with a **single diagram**
- Extrudes the 2D polar curve by rotating it 360° around the Z-axis
- Produces a surface of revolution, assuming the pattern is azimuthally symmetric

---

## Technical Highlights

- **C++17** – uses `std::clamp`, structured bindings, `if constexpr`
- **Clean architecture** – split into `parser`, `renderer2d`, `renderer3d`, and `ui` modules, each with a documented header
- **Cross-platform build** – CMake 3.16+ with automatic Raylib 5.0 fetch via `FetchContent`; tested on Windows 10/11 and Ubuntu 22.04
- **Robust parsing** – two-pass file reader; NaN sentinel approach handles mixed text/numeric files without hard-coded line counts
- **Sub-index HPBW** – linear interpolation between samples for fractional-degree precision on beamwidth calculation
- **Zero heap allocation in render loop** – mesh vertex vectors are stack-local per frame to avoid allocator pressure at 60 FPS

---

## Screenshots

> *(Add screenshots of your 2D polar plot and 3D spherical view here)*

---

## Project Structure

```
Pfc/
├── src/
│   ├── main.cpp          # Entry point, application state machine
│   ├── parser.cpp        # .atn file reader and data normalization
│   ├── renderer2d.cpp    # 2D polar plot rendering
│   ├── renderer3d.cpp    # 3D spherical / rotation rendering
│   └── ui.cpp            # Splash screen, menu, error screen
├── include/
│   ├── parser.h
│   ├── renderer2d.h
│   ├── renderer3d.h
│   └── ui.h
├── assets/
│   ├── background.jpg
│   └── pic1.jpg
├── deps/
│   ├── raygui.h              # Single-header GUI (Raylib ecosystem)
│   └── tinyfiledialogs/      # Native OS file picker
│       ├── tinyfiledialogs.h
│       └── tinyfiledialogs.c
├── data/
│   └── samples/              # Example .atn files
├── CMakeLists.txt
├── .gitignore
└── README.md
```

---

## Building

### Prerequisites

| Tool | Version |
|---|---|
| CMake | ≥ 3.16 |
| C++ compiler | GCC 10+, Clang 12+, or MSVC 2019+ |
| tinyfiledialogs | [Download here](https://sourceforge.net/projects/tinyfiledialogs/) – place `tinyfiledialogs.h` and `tinyfiledialogs.c` in `include/` |

Raylib 5.0 is fetched automatically by CMake if not already installed.

### Linux / macOS

```bash
git clone https://github.com/AyaMrm/Pfc.git
cd Pfc

# Place tinyfiledialogs files in include/ first

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

The binary is generated in `build/`. Assets are copied automatically to `build/assets/`.

### Windows (MinGW)

```powershell
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make
```

### Windows (Visual Studio)

```bash
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

The binary is generated in `build/Release/`.

### Run

From the `build` directory:

```powershell
.\Graphix.exe
```

---

## Usage

1. Launch the application
2. On the menu, click **Browse** and select your `.atn` file
3. Choose a visualization mode:
   - **2D Polar Plot** – works with 1 or more diagrams
   - **3D Smoothed Surface** – requires at least 2 diagrams (E-plane + H-plane)
   - **3D Rotation** – works with a single diagram
4. Press **ESC** to return to the menu at any time

### Keyboard shortcuts

| Key | Action |
|---|---|
| `ESC` | Return to main menu |
| `SPACE` | Toggle orbital / free camera (3D modes) |
| `T` | Top view (free camera) |
| `B` | Bottom view (free camera) |
| `WASD` + mouse | Navigate in free camera mode |

---

## Results and Limitations

The software successfully generates:
- Clear 2D polar radiation graphs with HPBW annotation
- Interactive 3D radiation surfaces with heat-map coloring

These results allow:
- Identification of radiation peaks and nulls
- Performance analysis of antennas
- Comparison between E-plane and H-plane patterns

**Known limitations:**
- 3D reconstruction quality depends on having accurate E-plane and H-plane data
- Very noisy or irregularly sampled data may produce visual artifacts
- No built-in filtering or smoothing of raw input data (planned for future versions)

---

## Future Improvements

- Support for additional file formats (CSV, JSON)
- Advanced data filtering and smoothing
- Automated report generation (PDF export)
- Side-by-side comparison of multiple radiation diagrams
- Performance optimization for large datasets
- Potential extensions to other domains (GPS antenna testing, medical imaging, environmental studies)

---

## References

- Balanis, C. A. *Antenna Theory: Analysis and Design* (4th ed.)
- Stutzman & Thiele, *Antenna Theory and Design*
- [Raylib Documentation](https://www.raylib.com/)

---

## License

This project is released under the [MIT License](LICENSE).

---

Supervisor: Dr. Lahmissi

> *Feel free to open an issue or submit a pull request.*