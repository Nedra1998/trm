#include <glm/gtc/matrix_transform.hpp>
#include <trm/trm.hpp>

using namespace trm;

void scene(Renderer &renderer) {
  renderer.camera = Camera{
      .pos = {0.0f, 0.0f, -6.0f},
      .center = {0.0f, 0.0f, 0.0f},
      .up = {0.0f, 1.0f, 0.0f},
      .fov = 1.5707f,
  };

  renderer.scene =
      Scene{.transforms_ =
                {
                    Mat4(1.0f),
                    Mat4(1.0f),
                    Mat4(1.0f),
                    Mat4(1.0f),
                    Mat4(1.0f),
                    Mat4(1.0f),
                },
            .inv_transforms_ =
                {
                    Mat4(1.0f),
                    Mat4(1.0f),
                    Mat4(1.0f),
                    Mat4(1.0f),
                    Mat4(1.0f),
                    Mat4(1.0f),
                },
            .materials_ =
                {
                    {Shading::DIFFUSE, Vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f},   // Floor
                    {Shading::EMISSIVE, Vec3(1.0f, 1.0f, 1.0f), 10.0f, 0.0f}, // Ceiling
                    {Shading::DIFFUSE, Hex("#F44336"), 0.0f, 0.0f},           // Front
                    {Shading::DIFFUSE, Hex("#4CAF50"), 0.0f, 0.0f},           // Right
                    {Shading::DIFFUSE, Hex("#FFEB3B"), 0.0f, 0.0f},           // Back
                    {Shading::DIFFUSE, Hex("#2196F3"), 0.0f, 0.0f},           // Front
                },
            .sdfs_ = {
                [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 1.0f, 0.0f)) - -3.5f; },   // Floor
                [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, -1.0f, 0.0f)) - -10.5f; }, // Ceiling
                [](Vec3 p) -> Float { return dot(p, Vec3(1.0f, 0.0f, 0.0f)) - -7.0f; },   // Left
                [](Vec3 p) -> Float { return dot(p, Vec3(-1.0f, 0.0f, 0.0f)) - -7.0f; },  // Right
                [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, 1.0f)) - -7.0f; },   // Back
                [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, -1.0f)) - -7.0f; },  // Front
            }};
}
