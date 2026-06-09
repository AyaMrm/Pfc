# Contributing

Contributions, bug reports, and feature suggestions are welcome.

## Reporting issues

Open a GitHub issue with:
- A clear title and description
- Steps to reproduce (if a bug)
- Your OS, compiler version, and CMake version

## Submitting a pull request

1. Fork the repository
2. Create a branch: `git checkout -b feature/my-feature`
3. Make your changes — keep each commit focused
4. Run the build on your platform to confirm nothing is broken
5. Open a pull request against `main` with a clear description of what changed and why

## Code style

- C++17 standard
- 4-space indentation, no tabs
- snake_case for variables and functions, PascalCase for types and namespaces
- Every public function documented with a Doxygen `@brief` comment in the header
- No raw `#define` macros for logic — prefer `constexpr` and `inline` functions
- Avoid `using namespace std` in headers

## Adding a new file format

Parser extensions go in `src/parser.cpp` — implement a private helper function and call it from `loadFile()` based on file extension detection.
