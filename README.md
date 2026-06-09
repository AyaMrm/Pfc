# Graphix â€” Antenna Radiation Pattern Viewer

> **Interactive 2D/3D visualization of antenna radiation diagrams from `.atn` measurement files.**
> Built in C++17 with [Raylib](https://www.raylib.com/) and RayGUI.

---

## Overview

Graphix is a desktop application that parses raw antenna measurement data and renders it as interactive polar and spherical plots. It was developed as a final-year engineering project (PFC 2024â€“2025) under the supervision of Dr. Lahmissi.

The tool targets engineers and researchers working in RF, telecommunications, and antenna design who need a lightweight, dependency-light alternative to MATLAB or specialized software for visualizing radiation patterns.

---

## Features

| Feature | Details |
|---|---|
| **File parsing** | Reads `.atn` and `.txt` measurement files, strips metadata (dates, timestamps, labels), extracts numeric data into individual radiation diagrams |
| **2D Polar Plot** | Renders up to two overlaid diagrams (E-plane / H-plane) with dB reference circles, angular labels, and automatic HPBW (-3 dB beamwidth) markers |
| **3D Smoothed Surface** | Builds a full spherical triangle mesh by interpolating E-plane and H-plane data with cosine/sine elevation weighting |
| **3D Rotation Mode** | Extrudes a single diagram by rotation around the Z-axis as stacked angular slices |
| **Heat-map coloring** | Blue â†’ Green â†’ Red gradient mapped to signal intensity across all 3D views |
| **Interactive camera** | Orbital (auto-rotate) and free camera modes (WASD + mouse drag) in 3D views |
| **Data normalization** | Peak set to 0 dB, diagram rotated so the main lobe is at 0Â° â€” consistent across all modes |

---

## Technical Highlights

- **C++17** â€” uses `std::clamp`, structured bindings, `if constexpr`
- **Clean architecture** â€” split into `parser`, `renderer2d`, `renderer3d`, and `ui` modules, each with a documented header
- **Cross-platform build** â€” CMake 3.16+ with automatic Raylib 5.0 fetch via `FetchContent`; tested on Windows 10/11 and Ubuntu 22.04
- **Robust parsing** â€” two-pass file reader; NaN sentinel approach handles mixed text/numeric files without hard-coded line counts
- **Sub-index HPBW** â€” linear interpolation between samples for fractional degree precision on beamwidth calculation
- **Zero heap allocation in render loop** â€” mesh vertex vectors are stack-local per frame to avoid allocator pressure at 60 FPS

---

## Screenshots

> *(Add screenshots of your 2D polar plot and 3D spherical view here)*

---

## Building

### Prerequisites

| Tool | Version |
|---|---|
| CMake | â‰¥ 3.16 |
| C++ compiler | GCC 10+, Clang 12+, or MSVC 2019+ |
| tinyfiledialogs | [Download here](https://sourceforge.net/projects/tinyfiledialogs/) â€” place `tinyfiledialogs.h` and `tinyfiledialogs.c` in `include/` |

Raylib 5.0 is fetched automatically by CMake if not already installed.

### Build steps

```bash
git clone https://github.com/AyaMrm/Pfc.git
cd Pfc

# Place tinyfiledialogs files in include/ first

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

The binary is generated in `build/` (or `build/Release/` on Windows).
Assets are copied automatically to `build/assets/`.

### Windows (MinGW)

If you are using MinGW Makefiles:

```powershell
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
mingw32-make
```

### Run

From the `build` directory:

```powershell
.\Graphix.exe
```
### Windows (Visual Studio)

```bash
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

---

## Project Structure

```
Pfc/
â”œâ”€â”€ src/
â”‚   â”œâ”€â”€ main.cpp          # Entry point, application state machine
â”‚   â”œâ”€â”€ parser.cpp        # .atn file reader and data normalization
â”‚   â”œâ”€â”€ renderer2d.cpp    # 2D polar plot rendering
â”‚   â”œâ”€â”€ renderer3d.cpp    # 3D spherical / rotation rendering
â”‚   â””â”€â”€ ui.cpp            # Splash screen, menu, error screen
â”œâ”€â”€ include/
â”‚   â”œâ”€â”€ parser.h
â”‚   â”œâ”€â”€ renderer2d.h
â”‚   â”œâ”€â”€ renderer3d.h
â”‚   â””â”€â”€ ui.h
â”œâ”€â”€ assets/
â”‚   â”œâ”€â”€ background.jpg
â”‚   â””â”€â”€ pic1.jpg
â”œâ”€â”€ deps/
â”‚   â”œâ”€â”€ raygui.h              # Single-header GUI (Raylib ecosystem)
â”‚   â””â”€â”€ tinyfiledialogs/      # Native OS file picker
â”‚       â”œâ”€â”€ tinyfiledialogs.h
â”‚       â””â”€â”€ tinyfiledialogs.c
â”œâ”€â”€ data/
â”‚   â””â”€â”€ samples/              # Example .atn files
â”œâ”€â”€ CMakeLists.txt
â”œâ”€â”€ .gitignore
â””â”€â”€ README.md
```

---

## Data File Format

The parser accepts `.atn` files structured as alternating blocks of metadata and numeric measurements:

```
15-03-2024          â† date (skipped)
08:30:00            â† time (skipped)
Antenna_XY          â† label (skipped)
-12.5               â† measurement value (kept)
-10.3
...
15-03-2024          â† next block separator
...
```

Each numeric block separated by metadata becomes one radiation diagram.

---

## Usage

1. Launch the application
2. On the menu, click **Browse** and select your `.atn` file
3. Choose a visualization mode:
   - **2D Polar Plot** â€” works with 1 or more diagrams
   - **3D Smoothed Surface** â€” requires at least 2 diagrams (E-plane + H-plane)
   - **3D Rotation** â€” works with a single diagram
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

## Background â€” Antenna Radiation Patterns

A radiation pattern describes how an antenna distributes transmitted power in space. Key concepts used in this project:

- **dB (decibel)** â€” logarithmic power ratio; allows large dynamic ranges to be displayed compactly
- **dBm** â€” power relative to 1 mW; used in RF link budget analysis
- **HPBW (Half-Power BeamWidth)** â€” the angular width of the main lobe at the -3 dB points; a key antenna specification
- **E-plane / H-plane** â€” the two principal planes of a linearly polarized antenna, used together to reconstruct a 3D pattern

References: Balanis, *Antenna Theory: Analysis and Design* (4th ed.); Stutzman & Thiele, *Antenna Theory and Design*.

---

## License

This project is released under the [MIT License](LICENSE).

---

## Author

**Aya Mrm** â€” Electronics & Telecommunications Engineering student  
University project, 2024â€“2025 Â· Supervisor: Dr. Lahmissi

> *Feel free to open an issue or submit a pull request.*
