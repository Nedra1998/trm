# Examples

TRM includs a number of example scenes that can provide an introduction for how
to use the renderer. Each of the examples are also used as benchmarks for the
renderer. As such data is collected for each example which can be used to
analyze the improvements of each feature.

## Example 1: Box Scene

This is the most basic example scene, it just consists of an empty box with a
ceiling light. The walls of the box are colored differently.

![box](box.png)

<details>
  <summary>Statistics</summary>
  <img src="box_samples_vs_time.png" />
  <img src="box_samples_vs_delta_e.png" />
</details>

## Example 2: Basic Scene

This example consists of the same box from the first example. Within the box is
a diffuse sphere, a reflective sphere, and a refractive torus. Since the scene
contains several different objects of the different materials it is a good
simple scene that tests the core features of the renderer.

![basic](basic.png)

<details>
  <summary>Statistics</summary>
  <img src="basic_samples_vs_time.png" />
  <img src="basic_samples_vs_delta_e.png" />
</details>

## Example 3: Spheres Scene

This example uses the same box as the first scene, but is filled with many
sphers of various sizes and materials. With the higher number of objects it
usually has the slowest rendering of the existing samples, and acts as a good
benchmark for performance.

![spheres](spheres.png)

<details>
  <summary>Statistics</summary>
  <img src="spheres_samples_vs_time.png" />
  <img src="spheres_samples_vs_delta_e.png" />
</details>

## Example 4: Menger Sponge

This example contains the menger sponge fractal. The menger sponge is a 3D
block fractal that is created by dividing a cube into 27 smaller cubes and 
then removing the center cube and the center cube of each face. This process
is then repeated for each of the remaining cubes. This scene consists of the
single object, but the computational complexity of thet object makes it a good
benchmark for efficiency tests.

![menger](menger.png)

<details>
  <summary>Statistics</summary>
  <img src="menger_samples_vs_time.png" />
  <img src="menger_samples_vs_delta_e.png" />
</details>
