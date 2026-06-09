cmake_minimum_required(VERSION 3.16)
project(Graphix VERSION 1.0.0 LANGUAGES C CXX)

# ---------------------------------------------------------------------------
# C++ standard
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Build type default
# ---------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

# --- Raylib (fetched automatically if not found locally) -------------------
find_package(raylib 5.0 QUIET)
if(NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.0
        GIT_SHALLOW    TRUE
    )
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)
endif()

# --- tinyfiledialogs (header + single C file) -----------------------------
#     tinyfiledialogs.h and tinyfiledialogs.c live in include/ in this repo.
set(TINYFD_DIR "${CMAKE_SOURCE_DIR}/include")
if(EXISTS "${TINYFD_DIR}/tinyfiledialogs.c")
    add_library(tinyfd STATIC "${TINYFD_DIR}/tinyfiledialogs.c")
    target_include_directories(tinyfd PUBLIC "${TINYFD_DIR}")
else()
    message(WARNING
        "tinyfiledialogs not found in ${TINYFD_DIR}.\n"
        "Download from https://sourceforge.net/projects/tinyfiledialogs/\n"
        "and place tinyfiledialogs.h + tinyfiledialogs.c in include/")
    # Stub so the rest of CMake continues
    add_library(tinyfd INTERFACE)
    target_compile_definitions(tinyfd INTERFACE TINYFD_MISSING)
endif()

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
set(SOURCES
    src/main.cpp
    src/parser.cpp
    src/renderer2d.cpp
    src/renderer3d.cpp
    src/ui.cpp
    src/raygui_impl.cpp   # single translation unit for RAYGUI_IMPLEMENTATION
)

add_executable(${PROJECT_NAME} ${SOURCES})

# ---------------------------------------------------------------------------
# Include paths
# ---------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    deps           # raygui.h, tinyfiledialogs.h
)

# ---------------------------------------------------------------------------
# Linking
# ---------------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE raylib tinyfd)

# Platform-specific extras
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework GLUT"
        "-framework OpenGL"
    )
else()   # Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE GL m pthread dl rt X11)
endif()

# ---------------------------------------------------------------------------
# Copy assets to build directory
# ---------------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copying assets..."
)

# ---------------------------------------------------------------------------
# Install rules (optional)
# ---------------------------------------------------------------------------
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY assets/ DESTINATION bin/assets)

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------
message(STATUS "---------------------------------------------")
message(STATUS "  Project   : ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Build     : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler  : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  C++       : ${CMAKE_CXX_STANDARD}")
message(STATUS "---------------------------------------------")
