<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Nedra1998/trm">
    <img src="docs/assets/logo.svg" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">TRM</h3>

  <p align="center">
    A tiny ray marcher in C++, built to measure what each feature is actually worth.
    <br />
    <br />
    <a href="docs/examples/EXAMPLES.md">Examples</a>
    ·
    <a href="docs/features/FEATURES.md">Feature Benchmarks</a>
    ·
    <a href="https://github.com/Nedra1998/trm/issues/new?labels=bug">Report Bug</a>
  </p>
</div>

<!-- ABOUT THE PROJECT -->

## About The Project

![Menger Sponge Example](docs/examples/menger.png)

TRM (Tiny Ray Marcher) is a physically based renderer that draws scenes made of
signed distance functions. It's written as a small C++23 library with a
handful of example scenes and a CLI for analyzing the output.

This is the second version of the project, rebuilt with a focus on data.
Every feature I add gets benchmarked against the same set of scenes, so I can
show with numbers how much faster it got and how much closer it gets to the ground truth per sample.

What's in the renderer today:

- Sphere tracing against arbitrary SDFs, with per-object transforms
- Monte Carlo path tracing with progressive rendering and snapshotting
- Emissive, diffuse (Lambertian), specular, refractive, and Fresnel materials
- Normals estimated from the SDF gradient by central differences
- OpenMP parallelism over pixels, with the chunk size [tuned by benchmark](docs/features/FEATURES.md)
- Color, albedo, normal, and depth output buffers
- Image comparison tools covering several error metrics plus CIE76 ΔE
- Tracy profiler integration behind a CMake flag

## Measuring Convergence

Render time is easy to measure. Image quality is harder. My approach:

1. Render each scene once at a very high sample count (around 1024 spp) to use
   as the reference.
2. Render it again with a different seed, saving a snapshot every 8 samples.
3. For each snapshot, compute the mean per-pixel ΔE (CIE76) against the
   reference.

ΔE is a perceptual color difference, so noise a person can't see doesn't count
against the score. A ΔE under about 2 is generally considered imperceptible,
which makes it a handy target line on the graphs. Plotting ΔE against sample
count shows how fast a given build converges, and comparing commits shows
whether a change actually helped.

<p align="center">
  <img src="docs/examples/spheres_samples_vs_time.png" width="49%" alt="Samples vs time for the spheres scene" />
  <img src="docs/examples/spheres_samples_vs_delta_e.png" width="49%" alt="Samples vs Delta E for the spheres scene" />
</p>

All of the raw data lives in [`data/`](data) as CSV, keyed by commit and seed,
and the graphs are generated with gnuplot from [`scripts/`](scripts).

## Example Scenes

Each scene is chosen to stress a different part of the renderer.

| | |
| :-: | :-: |
| ![Box](docs/examples/box.png) | ![Basic](docs/examples/basic.png) |
| **Box.** An empty room lit by the ceiling. This is the best case baseline. | **Basic.** The room plus a diffuse sphere, a mirror sphere, and a glass torus. Covers every material type. |
| ![Spheres](docs/examples/spheres.png) | ![Menger](docs/examples/menger.png) |
| **Spheres.** 20 randomized spheres with a mix of materials. The scene that will benefit most from a BVH. | **Menger.** A fractal sponge with lots of fine detail and deep bounces. Hard for a ray tracer, natural for a ray marcher. |

There's also an infinite Menger sponge (domain repetition makes it nearly free
to build) and a glass cube. More detail and per-scene graphs are in
[EXAMPLES.md](docs/examples/EXAMPLES.md).

## Getting Started

### Requirements

- CMake 3.22+
- A C++23 compiler (GCC 14+ or Clang 18+)
- OpenMP (optional, on by default)
- Doxygen and Graphviz (only for building the docs)

Everything else (fmt, spdlog, glm, argparse, Catch2, Tracy) is fetched by
[CPM](https://github.com/cpm-cmake/CPM.cmake) at configure time.

### Build

```sh
git clone https://github.com/Nedra1998/trm.git
cd trm
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Useful options:

| Option                   | Default | Description                    |
| ------------------------ | ------- | ------------------------------ |
| `TRM_USE_OPENMP`         | `ON`    | Parallelize rendering          |
| `TRM_BUILD_EXAMPLES`     | `ON`    | Build the example scenes       |
| `TRM_BUILD_TESTS`        | `ON`    | Build the Catch2 unit tests    |
| `TRM_BUILD_DOCS`         | `OFF`   | Build the Doxygen site         |
| `TRM_ENABLE_PROFILING`   | `OFF`   | Enable Tracy instrumentation   |

### Render a scene

Each example builds into its own executable named `example-<scene>`.

```sh
./build/examples/example-menger --samples 256 --batch 8 --resolution 1024 1024
```

Snapshots are written to `output/` every batch, and timing data is appended to
`output/menger.csv`.

### Compare against a reference

The `trm` binary takes a reference image and the CSV from a render, and adds
the error metrics for every snapshot.

```sh
./build/bin/trm error reference.png output/menger.csv -o report.csv
./build/bin/trm diff reference.png output/menger_..._0064.png -m squared-error
```

## Using the Library

A scene is just a list of SDFs with matching materials and transforms. Here's
a single glowing sphere over a floor:

```cpp
#include <trm/trm.hpp>

using namespace trm;

int main() {
  Renderer renderer{
      .cfg = Config{.max_depth = 16, .max_steps = 256, .max_distance = 1e3f,
                    .epsilon = 1e-5f, .bias = 5e-5f},
      .camera = Camera{.pos = {0, 0, -6}, .center = {0, 0, 0}, .up = {0, 1, 0}, .fov = 1.57f},
      .scene = Scene{
          .transforms_ = {Mat4(1.0f), Mat4(1.0f)},
          .inv_transforms_ = {Mat4(1.0f), Mat4(1.0f)},
          .materials_ = {{Shading::DIFFUSE, Vec3(1.0f), 0.0f},
                         {Shading::EMISSIVE, Hex("#FFB74D"), 5.0f}},
          .sdfs_ = {[](Vec3 p) { return p.y + 1.0f; },
                    [](Vec3 p) { return length(p) - 1.0f; }},
      },
  };

  ImageSet img(512, 512);
  renderer.render("sphere_{samples:04d}.png", img, 64, 8, Buffer::COLOR);
}
```

Link against `trm::trm` in CMake and you're set. The full API reference can be
generated with `-DTRM_BUILD_DOCS=ON` and `cmake --build build --target docs`.

<!-- LICENSE -->

## License

Distributed under the MIT License. See [`LICENSE`](LICENSE) for more information.
