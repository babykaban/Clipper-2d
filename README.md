# Clipper 2d

## Overview
This repository contains a custom implementation of a portion of the [Clipper2](https://github.com/AngusJohnson/Clipper2) library.
Initially, the goal was to fully reimplement Clipper2 to enhance its performance and efficiency. However, given that Clipper2 is actively
maintained, this project evolved into a streamlined version tailored for personal use within a game engine.

## Purpose
The primary objective of this implementation is to perform boolean operations on polygons efficiently. While maintaining core functionality,
this version introduces multiple optimizations and structural changes to improve speed and memory usage.

## Key Enhancements

### Data Structure Modifications
- Replaced C++ `std::vector` with standard C arrays for better control over memory.
- Removed templates to simplify the implementation and reduce complexity.
- Excluded unnecessary features such as **PolyTrees** and **USINGZ** to streamline operations.

### Memory Management Optimization
- Introduced a **freelist** mechanism for `actives` and `out_pt` to reduce the number of system calls for memory allocation and deallocation.
- Implemented **block allocation** instead of allocating memory individually, improving performance and reducing fragmentation.
- Enhanced dynamic array allocation:
  - Arrays are initially allocated with a predefined size.
  - When the number of elements exceeds the allocated capacity, the array is resized dynamically.

### Sorting Efficiency
- Replaced the standard `std::quicksort` with a more optimized sorting approach:
  - **Insertion sort** is used for small arrays due to its efficiency with a low number of elements.
  - If the number of elements surpasses a specific threshold, **MergeSort** is employed to maintain optimal performance.

### SIMD-Based Performance Boost
- Leveraged **SIMD (Single Instruction, Multiple Data) instruction sets** to optimize various functions, including **path scaling**.
- Utilized **double-precision floating points** and **64-bit integers** for mathematical computations.
- Represented **2D points (double type)** as a single **128-bit register**, enabling highly efficient vector operations such as addition, subtraction, and multiplication.

## Usage
This library is designed for direct integration into my game engine, focusing on efficiency and optimized memory management
while preserving the essential boolean polygon operations.

## Future Development
Further refinements and performance optimizations may be implemented based on benchmarks and additional project requirements.

## Credits
This implementation is inspired by [Clipper2](https://github.com/AngusJohnson/Clipper2), a well-maintained and powerful polygon clipping library.
