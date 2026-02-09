# Compile-Time Ray Tracer - A Program That Only Exists as a Compiler Error

## Concept

Create a C++20/23 ray tracer that performs all rendering at compile-time and outputs the result as ASCII art through compiler error messages. The program intentionally fails to compile, with the rendered 3D scene embedded in the static_assert failure messages. The computation happens during compilation; there is no successful executable.

## Core Requirements

### 1. Constexpr Ray Tracing Engine

Implement a complete ray tracer using only constexpr functions:

- **Vec3 class**: 3D vector with constexpr operations (add, sub, dot, cross, normalize, length)
- **Ray class**: Origin and direction
- **Sphere class**: Center, radius, material properties (color, reflectivity)
- **Intersection testing**: Ray-sphere intersection, all constexpr
- **Shading**: Lambertian diffuse, specular highlights, shadows
- **Scene definition**: Array of spheres, light sources, camera parameters

All computation must be constexpr-compatible (no dynamic allocation, no floating-point operations that aren't constexpr in C++20).

### 2. ASCII Rendering

Convert the computed image to ASCII art:

- **Resolution**: 80x40 characters (or configurable via template parameters)
- **Character palette**: Map brightness to ASCII characters, e.g., ` .:-=+*#%@` (dark to bright)
- **Constexpr string generation**: Build the entire ASCII image as a compile-time string
- **Line breaks**: Proper formatting with newlines between rows

### 3. Compile-Time Output via static_assert

**Primary Approach** (C++20/23 compatible):
Generate multiple `static_assert(false, "line")` statements, one per line of output:
```cpp
static_assert(false, "  ****  ");
static_assert(false, " *    * ");
static_assert(false, "*  ()  *");
// ... one per line
```

Use template metaprogramming or macro generation to create 40 separate asserts.

**Alternative Approach** (if C++26 available):
```cpp
constexpr auto image = render_ascii<80, 40>();
static_assert(false, image.c_str());
```

**Fallback Approach** (using #pragma message):
Non-standard but widely supported:
```cpp
#pragma message("line 1")
#pragma message("line 2")
```

## Implementation Details

### Scene Setup

Create a simple but recognizable 3D scene:
- 3-5 spheres at different positions
- At least one light source
- Shadows for depth perception
- Optional: reflective sphere for visual interest

### Ray Tracing Algorithm

Standard ray tracing with:
1. For each pixel, cast ray from camera through pixel
2. Test intersection with all spheres
3. Find closest intersection
4. Calculate shading at hit point (diffuse + specular)
5. Cast shadow ray to light source
6. Convert final color to brightness value (0.0-1.0)
7. Map brightness to ASCII character

### Brightness Mapping
```cpp
constexpr char brightness_to_char(float b) {
    constexpr const char* palette = " .:-=+*#%@";
    int index = static_cast<int>(b * 9.99f);
    return palette[std::clamp(index, 0, 9)];
}
```

### Template Metaprogramming for Output

Use fold expressions or recursive templates to generate multiple static_asserts:
```cpp
template<size_t... Is>
constexpr void emit_lines(std::index_sequence<Is...>) {
    (static_assert(false, get_line<Is>()), ...);
}

// Invoke with:
emit_lines(std::make_index_sequence<HEIGHT>{});
```

Each `get_line<N>()` returns a compile-time string literal for line N.

## Success Criteria

When you run `g++ -std=c++20 raytracer.cpp`, the output should:

1. **Fail to compile** (intentionally)
2. **Display ASCII art** of the rendered scene in the error messages
3. **Show compilation time** (likely 10-60 seconds depending on scene complexity)
4. **Be recognizable**: Spheres visible, shadows present, depth apparent

Example compiler output:
```
raytracer.cpp:123: error: static assertion failed
                        .........
raytracer.cpp:124: error: static assertion failed
                     ...:::::::::...
raytracer.cpp:125: error: static assertion failed
                   ...:::######:::...
raytracer.cpp:126: error: static assertion failed
                  ...:::#@@@@@@#:::...
```

## Optional Extensions

### 1. Multiple Scenes
Define several scenes and compile-time select which to render via template parameter.

### 2. Animation Frames
Generate multiple frames at compile time, each as a separate error message set.

### 3. Reflections
Add recursive ray tracing for mirror-like spheres (watch constexpr evaluation depth limits).

### 4. Compile-Time Metrics
Use `static_assert(true, "Scene has N spheres")` to output metadata.

### 5. Binary Size Analysis
Since no executable is produced, examine the .o file size to see template instantiation costs.

### 6. Compiler Comparison
Test on GCC, Clang, MSVC - compare compile times and error message formatting.

## Technical Constraints

- **No iostream**: Can't use std::cout (runtime I/O)
- **No file I/O**: Can't write files at compile-time
- **Constexpr limits**: Watch recursion depth (typically 512) and evaluation step limits
- **No exceptions**: Not allowed in constexpr context
- **No dynamic memory**: std::vector not constexpr-friendly in C++20
- **Fixed-size arrays**: Use std::array or C arrays

## Recommended Structure
```cpp
// raytracer.cpp

#include <array>
#include <cmath>

// Vec3, Ray, Sphere classes with constexpr everything
// ...

// Ray tracing logic
constexpr float trace_ray(/* ... */) { /* ... */ }

// Render entire scene to brightness array
constexpr auto render_scene() {
    std::array<std::array<float, WIDTH>, HEIGHT> pixels{};
    // For each pixel, trace ray and compute brightness
    return pixels;
}

// Convert to ASCII
constexpr auto render_ascii() {
    auto pixels = render_scene();
    std::array<const char*, HEIGHT> lines;
    // Convert each row of pixels to string literal
    return lines;
}

// Generate static_asserts
template<size_t N>
constexpr const char* get_line() {
    constexpr auto lines = render_ascii();
    return lines[N];
}

template<size_t... Is>
void display(std::index_sequence<Is...>) {
    (static_assert(false, get_line<Is>()), ...);
}

int main() {
    display(std::make_index_sequence<HEIGHT>{});
}
```

## Compilation
```bash
g++ -std=c++20 -fconstexpr-steps=10000000 raytracer.cpp 2>&1 | head -100
```

Or:
```bash
clang++ -std=c++20 -fconstexpr-steps=10000000 raytracer.cpp
```

Adjust `-fconstexpr-steps` as needed for complex scenes.

## The Philosophical Hook

"This program computes a 3D rendered scene but never executes. It exists only as a compiler error. The image is calculated during compilation and displayed through failed static assertions. Zero runtime cost because there is no runtime."

Perfect for LinkedIn: visually striking, technically deep, philosophically interesting, easy to understand the concept.
