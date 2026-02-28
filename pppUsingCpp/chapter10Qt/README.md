A Qt6 port of the graphics library from [Programming: Principles and Practice Using C++](https://github.com/BjarneStroustrup/Programming-_Principles_and_Practice_Using_Cpp) by Bjarne Stroustrup.

Original Qt code: https://github.com/villevoutilainen/ProgrammingPrinciplesAndPracticeUsingQt/tree/main

## Dependencies

- Qt6
- CMake 3.5+
- C++20

## Install Qt6 (Ubuntu)

```bash
sudo apt install qt6-base-dev qt6-base-dev-tools
```

## Build

```bash
cmake . && make install
```

The compiled binary is placed at `build/ch10`.

## Run

```bash
./build/ch10
```
