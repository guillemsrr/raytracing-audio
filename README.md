# Raytracing Audio 🔊🌟

![C++](https://img.shields.io/badge/c++-20-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.20+-orange.svg)
![SDL3](https://img.shields.io/badge/SDL-3.0-green.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-Core-blue.svg)

The idea was to create a custom-built C++ core framework designed to unify optical and acoustic phenomena through real-time raytracing.
By leveraging a single mathematical foundation, it simulates both the behavior of light (Whitted/Path Tracing) and the propagation of sound through virtual environments.
But it is still WIP, just created the raytracing part, no audio yet :)

You can see the devlog on my [portfolio website](https://guillemserra.cat/projects/raytracing).

![Raytraced two-sphere render](https://pub-43bd1a2c92284948a57f7a70decf6fd6.r2.dev/images/projects/raytracing/two_spheres_improved.png)

## 🛠️ Technology Stack

- **Language:** C++20
- **Framework:** SerraEngine (Custom core engine)
- **Windowing & Input:** SDL3
- **Graphics API:** OpenGL (via GLAD)
- **Math:** GLM
- **Build System:** CMake (3.20+)

## 🏗️ Architectural Boundaries

The engine is engineered with strict adherence to **SOLID principles** and modular design:

- **Core Orchestration:** High-level minimal loop, decoupled from the underlying renderer.
- **Agnostic Renderer:** The pixel/sample buffer is oblivious to whether it's rendering optical photons or acoustic phonons.
- **Material System:** Unified `Material` structures that contain both optical parameters (Reflectance, Refraction) and acoustic coefficients (Absorption, Scattering).
- **Modern Audio Pipeline:** Utilizes the new SDL3 audio stream API (`SDL_GetAudioStreamData`) instead of legacy audio callbacks for seamless, low-latency playback.

## 🚀 Building from Source

### Prerequisites

- CMake 3.20 or higher
- A C++20 compatible compiler (MSVC, GCC, or Clang)
- [SDL3](https://github.com/libsdl-org/SDL)
- OpenGL development libraries

### Build Instructions

```bash
# Clone the repository and the SerraEngine dependency
git clone https://github.com/yourusername/raytracing-audio.git
cd raytracing-audio

# Create a build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . --config Release
```

## 📜 Development Guidelines & Coding Standards

- **Memory Management:** Strictly modern C++ idioms (`std::unique_ptr`, `std::shared_ptr`). Zero raw `new`/`delete` allocations in the high-level logic.
- **Composition over Inheritance:** The `Entity` and `Scene` structures are built to remain lightweight and extensible.
- **Performance:** Hot paths are profiled and optimized for cache coherency. Interface segregation guarantees the `Raytracer` logic never depends on external APIs like `SDL_Window`.

## 📚 Acknowledgements & References

The project has been inspired by various learning resources:

- CPU Raytracing: [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- Compute Shaders: Jakubg05, Kidrigger, LWJGL Wiki
- Vulkan Raytracing: TheCherno

## 📄 License

This project is open-source and available under the terms of the MIT License.
