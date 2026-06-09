#include "parser.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

namespace parser {

bool isRealNumber(const std::string& str) {
    if (str.empty()) return false;

    bool hasDot    = false;
    bool hasDigit  = false;

    for (size_t i = 0; i < str.size(); ++i) {
        const char c = str[i];

        if (i == 0 && c == '-') continue;   // leading minus is valid

        if (c == '.') {
            if (hasDot) return false;        // two dots → invalid
            hasDot = true;
        } else if (c >= '0' && c <= '9') {
            hasDigit = true;
        } else {
            return false;
        }
    }
    return hasDigit;                         // must have at least one digit
}

bool isDate(const std::string& str) {
    // Expected format: DD-MM-YYYY  (length = 10)
    if (str.size() != 10) return false;
    if (str[2] != '-' || str[5] != '-') return false;

    for (size_t i = 0; i < 10; ++i) {
        if (i == 2 || i == 5) continue;
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

bool isTime(const std::string& str) {
    // Expected format: HH:MM:SS  (length = 8)
    if (str.size() != 8) return false;
    if (str[2] != ':' || str[5] != ':') return false;

    for (size_t i = 0; i < 8; ++i) {
        if (i == 2 || i == 5) continue;
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}


GraphData loadFile(const std::string& filepath) {
    GraphData result;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[parser] Cannot open file: " << filepath << "\n";
        return result;
    }

    // Pass 1: build a flat vector of doubles, inserting a sentinel (NaN) each
    // time we encounter a non-numeric token (date, time, label, empty line).
    // Two consecutive sentinels are collapsed into one.
    std::vector<double> flat;
    bool lastWasSentinel = true;   // start as true to skip a leading sentinel
    std::string line;

    while (std::getline(file, line)) {
        // Trim CR / trailing whitespace
        while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
            line.pop_back();

        if (isRealNumber(line)) {
            flat.push_back(std::stod(line));
            lastWasSentinel = false;
        } else {
            if (!lastWasSentinel) {
                flat.push_back(std::numeric_limits<double>::quiet_NaN());
                lastWasSentinel = true;
            }
        }
    }
    file.close();

    // Pass 2: split flat vector on NaN sentinels → individual diagrams
    std::vector<double> current;
    for (double v : flat) {
        if (std::isnan(v)) {
            if (!current.empty()) {
                result.diagrams.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(v);
        }
    }
    if (!current.empty()) result.diagrams.push_back(current);

    if (result.diagrams.empty()) {
        std::cerr << "[parser] No numeric data found in: " << filepath << "\n";
        return result;
    }

    // -----------------------------------------------------------------------
    // Pass 3: compute statistics, normalize, and rotate each diagram
    // -----------------------------------------------------------------------
    for (auto& diag : result.diagrams) {
        if (diag.empty()) continue;

        double maxVal = diag[0], minVal = diag[0];
        size_t maxIdx = 0;

        for (size_t i = 0; i < diag.size(); ++i) {
            if (diag[i] > maxVal) { maxVal = diag[i]; maxIdx = i; }
            if (diag[i] < minVal)   minVal = diag[i];
        }

        double range = maxVal - minVal;

        std::vector<double> stats = {
            static_cast<double>(diag.size()),
            maxVal,
            minVal,
            range,
            static_cast<double>(maxIdx)
        };
        result.characteristics.push_back(stats);

        // Normalize: shift so peak = 0 dB
        for (double& v : diag) v -= maxVal;

        // Rotate: bring peak index to position 0 (front)
        std::rotate(diag.begin(), diag.begin() + maxIdx, diag.end());
    }

    std::cout << "[parser] Loaded " << result.diagrams.size()
              << " diagram(s) from " << filepath << "\n";
    return result;
}

} // namespace parser
