#pragma once

#include <string>
#include <vector>

/**
 * @file parser.h
 * @brief Parsing and preprocessing of antenna measurement data files (.atn)
 *
 * Handles reading, validating and normalizing raw measurement data
 * stored in .atn or plain-text formats used by antenna measurement tools.
 */

namespace parser {

/**
 * @brief Holds a single radiation pattern dataset.
 *        Each sub-vector is one radiation diagram (e.g. E-plane, H-plane).
 */
struct GraphData {
    std::vector<std::vector<double>> diagrams;               ///< Normalized radiation diagrams
    std::vector<std::vector<double>> characteristics;        ///< Per-diagram stats: [size, max, min, range, maxIndex]
};

/**
 * @brief Checks whether a string represents a valid real number (including negatives).
 * @param str Input string
 * @return true if parseable as a floating-point number
 */
bool isRealNumber(const std::string& str);

/**
 * @brief Checks whether a string matches the date format DD-MM-YYYY.
 * @param str Input string
 * @return true if it matches the expected date format
 */
bool isDate(const std::string& str);

/**
 * @brief Checks whether a string matches the time format HH:MM:SS.
 * @param str Input string
 * @return true if it matches the expected time format
 */
bool isTime(const std::string& str);

/**
 * @brief Loads and preprocesses an .atn measurement file.
 *
 * Processing pipeline:
 *  1. Read lines, classify as numeric data or metadata (date/time/text)
 *  2. Split data stream into individual radiation diagrams
 *  3. Compute per-diagram statistics (max, min, range, peak index)
 *  4. Normalize each diagram: subtract peak value so max = 0 dB
 *  5. Rotate diagram so peak is at index 0 (front of vector)
 *
 * @param filepath Path to the .atn or .txt data file
 * @return GraphData structure with all diagrams and their statistics.
 *         Returns empty GraphData if the file cannot be opened.
 */
GraphData loadFile(const std::string& filepath);

} // namespace parser
