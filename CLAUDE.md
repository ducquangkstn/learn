# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Purpose

Personal learning repository. Currently contains exercises from *Programming: Principles and Practice Using C++* (PPP) by Bjarne Stroustrup.

## Structure

- `pppUsingCpp/chapterN/` — One directory per chapter, each is a self-contained CMake project.

## Build (per chapter)

Each chapter directory has its own `CMakeLists.txt`. To build:

```bash
# From the chapter directory (e.g., pppUsingCpp/chapter10/)
cmake -S . -B build && cmake --build build

# Or via the provided makefile shortcut
make install
```

The compiled binary is placed in `build/` (e.g., `build/ch10`).

## Dependencies

- **FLTK** — required for GUI chapters (e.g., chapter10). Must be installed system-wide so CMake's `find_package(FLTK REQUIRED)` can locate it.
- **C++17** — all projects use `set(CMAKE_CXX_STANDARD 17)`.

## Adding a New Chapter

Create a new directory `pppUsingCpp/chapterN/` with its own `CMakeLists.txt` following the same pattern as chapter10.
