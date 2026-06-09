/**
 * @file raygui_impl.cpp
 * @brief Single translation unit that instantiates the raygui implementation.
 *
 * raygui.h is a "single-header" library: it contains both declarations and
 * definitions. The definitions are gated behind RAYGUI_IMPLEMENTATION and must
 * be compiled exactly ONCE across the whole project.
 *
 * Rule: every other .cpp that uses raygui just does:
 *   #include "raygui.h"      <-- declarations only (no #define before it)
 *
 * Only THIS file defines RAYGUI_IMPLEMENTATION.
 */

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
