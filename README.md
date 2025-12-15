## Overview

This project started as a simple CPU ray tracer based on *Ray Tracing in One Weekend*, used primarily as a reference for basic ray–object intersection maths. From there, it evolved into a personal sandbox for experimenting with structure, performance, and parallel execution. Supports basic diffuse, metal and transparent materials.

## Notes and Extensions

- Originally developed on a Linux virtual machine, later moved to a dual-boot setup to allow full use of system CPU resources.

- The overall structure and coding style from *Ray Tracing in One Weekend* was not kept. It was mainly useful as a baseline for sphere intersection maths.

- Replaced the original math utilities with a custom `vec` template to simplify vector operations and improve code clarity.

- Uses a custom Python build script instead of `make`:
  - Supports multi-threaded compilation of `.cpp` files
  - Performs single-threaded linking
  - Rebuilds only changed source and header dependencies
  - Chosen to avoid treating the build system as a black box and to allow full control over build structure
  - I have better control over a build failure
  - I always find codium (vscode) is nicer to compile with using a python program over the built in tools. Having F5 simply launch a python program over creating a build task with the .json files.

- Added axis-aligned bounding boxes (AABBs), including cube intersection support.

- Implemented fast AABB intersection logic as part of a Bounding Volume Hierarchy (BVH), reducing per-ray scene intersection checks from O(n) to approximately O(log n) in typical cases.

- Added multi-threaded rendering. Thread count scales with available system cores and can be configured. Note that unrestricted settings can fully saturate the CPU, not as much as a CPU burner test or anything.
