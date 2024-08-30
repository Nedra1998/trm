#define GLM_FORCE_SWIZZLE

#include <glm/gtc/matrix_transform.hpp>
#include <trm/trm.hpp>

#include "example.hpp"

using namespace glm;
using namespace trm;

Camera CAMERA = Camera{
    .pos = {-3.0f, 7.0f, -6.0f},
    .center = {0.0f, 0.0f, 0.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .fov = 1.5707f,
};

Scene SCENE =
    Scene{.transforms_ =
              {
                  Mat4(1.0f),
                  Mat4(1.0f),
                  Mat4(1.0f),
                  Mat4(1.0f),
                  Mat4(1.0f),
                  Mat4(1.0f),
                  translate(Mat4(1.0f), Vec3(-4.06f, -2.8f, -6.30f)),
                  translate(Mat4(1.0f), Vec3(-4.30f, -2.0f, -0.21f)),
                  translate(Mat4(1.0f), Vec3(4.19f, -2.9f, -5.07f)),
                  translate(Mat4(1.0f), Vec3(3.32f, -1.0f, 3.85f)),
                  translate(Mat4(1.0f), Vec3(2.26f, -2.3f, -4.49f)),
                  translate(Mat4(1.0f), Vec3(-5.48f, -2.6f, -3.92f)),
                  translate(Mat4(1.0f), Vec3(-1.73f, -1.4f, 3.93f)),
                  translate(Mat4(1.0f), Vec3(4.00f, -1.7f, -0.83f)),
                  translate(Mat4(1.0f), Vec3(-6.11f, -2.8f, -1.81f)),
                  translate(Mat4(1.0f), Vec3(-1.41f, -2.6f, -2.43f)),
                  translate(Mat4(1.0f), Vec3(-5.81f, -2.6f, 4.19f)),
                  translate(Mat4(1.0f), Vec3(-1.81f, -2.4f, -4.42f)),
                  translate(Mat4(1.0f), Vec3(0.62f, -2.1f, 0.41f)),
                  translate(Mat4(1.0f), Vec3(-5.67f, -2.9f, -5.54f)),
                  translate(Mat4(1.0f), Vec3(4.47f, -2.6f, -3.59f)),
                  translate(Mat4(1.0f), Vec3(-1.89f, -3.0f, -1.11f)),
                  translate(Mat4(1.0f), Vec3(6.01f, -2.8f, 6.06f)),
                  translate(Mat4(1.0f), Vec3(6.29f, -2.9f, -4.56f)),
                  translate(Mat4(1.0f), Vec3(-4.30f, -2.9f, 6.16f)),
                  translate(Mat4(1.0f), Vec3(-1.66f, -2.9f, 0.98f)),
              },
          .inv_transforms_ =
              {
                  Mat4(1.0f),
                  Mat4(1.0f),
                  Mat4(1.0f),
                  Mat4(1.0f),
                  Mat4(1.0f),
                  Mat4(1.0f),
                  translate(Mat4(1.0f), -Vec3(-4.06f, -2.8f, -6.30f)),
                  translate(Mat4(1.0f), -Vec3(-4.30f, -2.0f, -0.21f)),
                  translate(Mat4(1.0f), -Vec3(4.19f, -2.9f, -5.07f)),
                  translate(Mat4(1.0f), -Vec3(3.32f, -1.0f, 3.85f)),
                  translate(Mat4(1.0f), -Vec3(2.26f, -2.3f, -4.49f)),
                  translate(Mat4(1.0f), -Vec3(-5.48f, -2.6f, -3.92f)),
                  translate(Mat4(1.0f), -Vec3(-1.73f, -1.4f, 3.93f)),
                  translate(Mat4(1.0f), -Vec3(4.00f, -1.7f, -0.83f)),
                  translate(Mat4(1.0f), -Vec3(-6.11f, -2.8f, -1.81f)),
                  translate(Mat4(1.0f), -Vec3(-1.41f, -2.6f, -2.43f)),
                  translate(Mat4(1.0f), -Vec3(-5.81f, -2.6f, 4.19f)),
                  translate(Mat4(1.0f), -Vec3(-1.81f, -2.4f, -4.42f)),
                  translate(Mat4(1.0f), -Vec3(0.62f, -2.1f, 0.41f)),
                  translate(Mat4(1.0f), -Vec3(-5.67f, -2.9f, -5.54f)),
                  translate(Mat4(1.0f), -Vec3(4.47f, -2.6f, -3.59f)),
                  translate(Mat4(1.0f), -Vec3(-1.89f, -3.0f, -1.11f)),
                  translate(Mat4(1.0f), -Vec3(6.01f, -2.8f, 6.06f)),
                  translate(Mat4(1.0f), -Vec3(6.29f, -2.9f, -4.56f)),
                  translate(Mat4(1.0f), -Vec3(-4.30f, -2.9f, 6.16f)),
                  translate(Mat4(1.0f), -Vec3(-1.66f, -2.9f, 0.98f)),
              },
          .materials_ =
              {
                  {Shading::DIFFUSE, Vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f},   // Floor
                  {Shading::EMISSIVE, Vec3(1.0f, 1.0f, 1.0f), 10.0f, 0.0f}, // Ceiling
                  {Shading::DIFFUSE, Hex("#F44336"), 0.0f, 0.0f},           // Front
                  {Shading::DIFFUSE, Hex("#4CAF50"), 0.0f, 0.0f},           // Right
                  {Shading::DIFFUSE, Hex("#FFEB3B"), 0.0f, 0.0f},           // Back
                  {Shading::DIFFUSE, Hex("#2196F3"), 0.0f, 0.0f},           // Front
                  {Shading::DIFFUSE, Vec3(0.96f, 0.26f, 0.21f), 0.0f, 0.0f},
                  {Shading::DIFFUSE, Vec3(0.0f, 0.59f, 0.53f), 0.0f, 0.0f},
                  {Shading::DIFFUSE, Vec3(0.25f, 0.32f, 0.71f), 0.0f, 0.0f},
                  {Shading::EMISSIVE, Vec3(0.25f, 0.32f, 0.71f), 5.0f, 0.0f},
                  {Shading::SPECULAR, Vec3(0.91f, 0.12f, 0.39f), 0.0f, 0.0f},
                  {Shading::REFRACTIVE, Vec3(0.0f, 0.59f, 0.53f), 0.0f, 1.5f},
                  {Shading::DIFFUSE, Vec3(0.55f, 0.76f, 0.29f), 0.0f, 0.0f},
                  {Shading::DIFFUSE, Vec3(0.91f, 0.12f, 0.39f), 0.0f, 0.0f},
                  {Shading::DIFFUSE, Vec3(1.0f, 0.76f, 0.02f), 0.0f, 0.0f},
                  {Shading::EMISSIVE, Vec3(0.8f, 0.86f, 0.22f), 5.0f, 0.0f},
                  {Shading::SPECULAR, Vec3(0.0f, 0.74f, 0.83f), 0.0f, 0.0f},
                  {Shading::REFRACTIVE, Vec3(0.4f, 0.23f, 0.72f), 0.0f, 1.5f},
                  {Shading::DIFFUSE, Vec3(0.012f, 0.66f, 0.96f), 0.0f, 0.0f},
                  {Shading::DIFFUSE, Vec3(0.91f, 0.12f, 0.39f), 0.0f, 0.0f},
                  {Shading::DIFFUSE, Vec3(0.3f, 0.69f, 0.31f), 0.0f, 0.0f},
                  {Shading::EMISSIVE, Vec3(0.96f, 0.26f, 0.21f), 5.0f, 0.0f},
                  {Shading::SPECULAR, Vec3(0.4f, 0.23f, 0.72f), 0.0f, 0.0f},
                  {Shading::REFRACTIVE, Vec3(1.0f, 0.76f, 0.02f), 0.0f, 1.5f},
                  {Shading::DIFFUSE, Vec3(0.61f, 0.15f, 0.69f), 0.0f, 0.0f},
                  {Shading::DIFFUSE, Vec3(0.25f, 0.32f, 0.71f), 0.0f, 0.0f},
              },
          .sdfs_ = {
              [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 1.0f, 0.0f)) - -3.5f; },   // Floor
              [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, -1.0f, 0.0f)) - -10.5f; }, // Ceiling
              [](Vec3 p) -> Float { return dot(p, Vec3(1.0f, 0.0f, 0.0f)) - -7.0f; },   // Left
              [](Vec3 p) -> Float { return dot(p, Vec3(-1.0f, 0.0f, 0.0f)) - -7.0f; },  // Right
              [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, 1.0f)) - -7.0f; },   // Back
              [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, -1.0f)) - -7.0f; },  // Front
              [](Vec3 p) -> Float { return length(p) - 0.65f; },
              [](Vec3 p) -> Float { return length(p) - 1.5f; },
              [](Vec3 p) -> Float { return length(p) - 0.55f; },
              [](Vec3 p) -> Float { return length(p) - 2.5f; },
              [](Vec3 p) -> Float { return length(p) - 1.2f; },
              [](Vec3 p) -> Float { return length(p) - 0.92f; },
              [](Vec3 p) -> Float { return length(p) - 2.1f; },
              [](Vec3 p) -> Float { return length(p) - 1.8f; },
              [](Vec3 p) -> Float { return length(p) - 0.74f; },
              [](Vec3 p) -> Float { return length(p) - 0.86f; },
              [](Vec3 p) -> Float { return length(p) - 0.95f; },
              [](Vec3 p) -> Float { return length(p) - 1.1f; },
              [](Vec3 p) -> Float { return length(p) - 1.4f; },
              [](Vec3 p) -> Float { return length(p) - 0.64f; },
              [](Vec3 p) -> Float { return length(p) - 0.93f; },
              [](Vec3 p) -> Float { return length(p) - 0.53f; },
              [](Vec3 p) -> Float { return length(p) - 0.7f; },
              [](Vec3 p) -> Float { return length(p) - 0.62f; },
              [](Vec3 p) -> Float { return length(p) - 0.63f; },
              [](Vec3 p) -> Float { return length(p) - 0.62f; },
          }};
