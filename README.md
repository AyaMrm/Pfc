# Visualization of Data in Polar and Spherical Coordinates

## Project Overview

This project focuses on the **study and development of a software tool for graphical visualization of data using polar and spherical coordinates**.  
The main objective is to provide an **interactive and dynamic visualization system** capable of representing complex scientific data in both **2D and 3D**, particularly for applications such as **antenna radiation patterns**.

The project was carried out as a **pluridisciplinary academic project** during the **2024–2025 academic year**, under the supervision of **Dr. Lahmissi**.

---

## Project Objectives

The software aims to:

- Import data from specialized files (notably `.atn` and text formats)
- Process and normalize raw measurement data
- Display data as:
  - **2D polar graphs**
  - **3D spherical visualizations**
- Allow **user interaction** such as rotation, zooming, and mode switching
- Facilitate the interpretation of physical and scientific phenomena

---

## Importance of Polar and Spherical Visualization

Polar and spherical coordinate systems are essential for representing phenomena with **angular, circular, or spatial characteristics**.  
They are widely used in domains such as:

- Antenna theory and telecommunications
- Radar systems
- Astronomy and physics
- Wave propagation analysis

Compared to Cartesian coordinates, these systems offer a **clearer and more intuitive representation** when dealing with directional and angular data.

---

## Antenna Radiation Pattern Representation

A major application of this project is the visualization of **antenna radiation diagrams**, which represent the **intensity of emitted signals in different directions**.

These diagrams help:
- Identify zones of maximum and minimum radiation
- Evaluate antenna performance
- Optimize coverage and signal direction

Polar coordinates are preferred for their visual clarity, while 3D representations provide a complete spatial understanding of radiation behavior.

---

## dB and dBm Concepts

The project makes extensive use of **decibels (dB)** and **decibel-milliwatts (dBm)**:

- **dB**: A logarithmic unit used to express power, intensity, or voltage ratios
- **dBm**: A power measurement relative to **1 milliwatt (mW)**

These units are fundamental in telecommunications and engineering to:
- Measure signal strength
- Evaluate gains and losses
- Analyze system performance

---

## Data Files and Structure

The software processes measurement data stored in:

- `.atn` files
- Text-based formats (`.txt`)

### File Characteristics:
- Numerical values representing intensity and angular measurements
- Optional metadata (antenna details, measurement conditions, units)
- Data organized in blocks corresponding to radiation diagrams

The structured format enables direct interpretation and efficient visualization.

---

## Data Reading Methodology

Data processing begins with:

- Opening and parsing the data file
- Validating numerical formats
- Extracting relevant values
- Storing data in vectors or matrices

The data is then:
- Segmented into sub-series
- Normalized
- Prepared for graphical rendering

---

## Data Processing Methodology

The processing algorithm includes:

1. Reading polar-coordinate data from files
2. Removing metadata and irrelevant information
3. Segmenting data into individual radiation patterns
4. Normalizing values by subtracting the maximum intensity
5. Reordering data to improve visualization clarity

This ensures consistent and interpretable graphical output.

---

## Visualization Techniques

### 2D Visualization (Polar)

- Data plotted in polar coordinates
- Radius represents signal intensity
- Angle represents direction
- Concentric circles indicate intensity levels

This mode offers a clear and intuitive view of radiation distribution.

### 3D Visualization (Spherical)

- Data projected into a 3D space
- Intensity represented by distance from the origin
- Interactive rotation allows full spatial exploration

This mode provides a comprehensive understanding of signal coverage in all directions.

---

## Technologies Used

### Programming Language
- **C++**
  - High performance
  - Fine memory management
  - Suitable for scientific computation and graphics

### Visualization Library
- **Raylib**
  - Efficient 2D and 3D rendering
  - Real-time visualization
  - Simple and well-documented

### User Interface
- **RayGUI**
  - Interactive buttons and menus
  - File selection
  - Mode switching (2D / 3D)
  - User-friendly and accessible design

---

## Results and Analysis

The software successfully generates:

- Clear 2D polar radiation graphs
- Interactive 3D radiation visualizations

The results allow:
- Identification of radiation peaks and nulls
- Performance analysis of antennas
- Comparison between theoretical and measured data

Limitations may arise from:
- Data quality
- Measurement precision
- Modeling assumptions

---

## Future Improvements

Several enhancements are proposed:

- Support for additional file formats (CSV, JSON)
- Advanced data filtering and smoothing
- Automated report generation
- Comparison of multiple radiation diagrams
- Performance optimization for large datasets
- Potential use in other domains (GPS, medical imaging, environmental studies)

---

## Conclusion

This project demonstrates the **effectiveness of polar and spherical visualization** for representing complex scientific data.  
By combining **C++**, **Raylib**, and structured data processing, the software provides a **robust, interactive, and intuitive visualization tool**.

The project lays a strong foundation for future extensions and highlights the importance of advanced visualization techniques in scientific and industrial applications.

---

## References

- Balanis, C. A. *Antenna Theory: Analysis and Design*
- Stutzman & Thiele, *Antenna Theory and Design*
- Raylib Documentation
- USB Specifications (USB-IF)
- Academic course materials
