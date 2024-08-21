#include "trm/version.hpp"
#define GLM_FORCE_SWIZZLE
#include <fstream>

#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/cfg/env.h>
#include <trm/trm.hpp>

using namespace trm;

int main(int argc, char *argv[]) {

  spdlog::cfg::load_env_levels();

  Renderer renderer;
  renderer.cfg = Config{.max_depth = 16,
                        .max_steps = 256,
                        .max_distance = 100.0f,
                        .epsilon = 0.001f,
                        .bias = 0.01f,
                        .features = Feature::PROGRESS_REPORT};
  renderer.camera = Camera{
      .pos = {0.0f, 0.0f, -6.0f},
      .center = {0.0f, 0.0f, 0.0f},
      .up = {0.0f, 1.0f, 0.0f},
      .fov = 1.5707f,
  };
  // clang-format off
  renderer.scene = Scene{
    .transforms_ = {
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      // translate(Mat4(1.0f), Vec3(4.0f, -0.5f, 4.0f)),
      // translate(Mat4(1.0f), Vec3(0.5, -2.0, 3.0)),
      // translate(rotate(Mat4(1.0f), -0.785375f, Vec3(1.0, 0.0, -1.0)), Vec3(-3.0, 0.0, 3.0))
    },
    .inv_transforms_ = {
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      Mat4(1.0f),
      // translate(Mat4(1.0f), -Vec3(4.0f, -0.5f, 4.0f)),
      // translate(Mat4(1.0f), -Vec3(-0.5, -2.0, 3.0)),
      // translate(rotate(Mat4(1.0f), 0.785375f, Vec3(1.0, 0.0, -1.0)), -Vec3(-3.0, 0.0, 3.0))
    },
    .materials_ = {
      {Shading::DIFFUSE,    Vec3(1.0f,    1.0f,    1.0f),    0.0f,  0.0f}, // Floor
      {Shading::EMISSIVE,   Vec3(1.0f,    1.0f,    1.0f),    10.0f, 0.0f}, // Ceiling
      {Shading::DIFFUSE,    Vec3(0.12941, 0.58823, 0.95294), 0.0f,  0.0f}, // Right
      {Shading::DIFFUSE,    Vec3(0.95686, 0.26274, 0.21176), 0.0f,  0.0f}, // Left
      {Shading::DIFFUSE,    Vec3(0.0f,    0.0f,    1.0f),    0.0f,  0.0f}, // Front
      {Shading::DIFFUSE,    Vec3(0.29803, 0.68627, 0.31372), 0.0f,  0.0f}, // Back
      // {Shading::DIFFUSE,    Vec3(1.0f,    1.0f,    1.0f),    0.0f,  0.0f},
      // {Shading::SPECULAR,   Vec3(1.0f,    1.0f,    1.0f),    0.0f,  0.0f},
      // {Shading::FRESNEL,    Vec3(1.0f,    1.0f,    1.0f),    0.0f,  1.5f},
    },
    .sdfs_ = {
      [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 1.0f, 0.0f)) - -3.5f; },   // Floor
      [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, -1.0f, 0.0f)) - -10.5f; }, // Ceiling
      [](Vec3 p) -> Float { return dot(p, Vec3(1.0f, 0.0f, 0.0f)) - -7.0f; },   // Right
      [](Vec3 p) -> Float { return dot(p, Vec3(-1.0f, 0.0f, 0.0f)) - -7.0f; },  // Left
      [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, 1.0f)) - -7.0f; },   // Front
      [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, -1.0f)) - -7.0f; },  // Back
      // [](Vec3 p) -> Float { return length(p) - 3.0f; },
      // [](Vec3 p) -> Float { return length(p) - 1.5f; },
      // [](Vec3 p) -> Float { return length(Vec2(length(p.xz()) - 3.0f, p.y)) - 0.75f; }
    }
  };
  // clang-format on

  ImageSet img(512, 512);
  const auto result =
      renderer.render("output/box/{commit}.{buffer}.{samples:03d}.png", img, 128, 8, true);

  std::fstream csv(fmt::format("output/box/{commit}.csv", fmt::arg("commit", TRM_VERSION_COMMIT)),
                   std::ios::out | std::ios::trunc);
  if (!csv.is_open()) {
    LOG_ERROR("Failed to open CSV file");
    return 1;
  }

  std::println(csv,
               "filename,total_time,batch_time,samples,batch_size,error,absolute,squared,relative_"
               "absolute,relative_squared");
  for (const auto snapshot : result) {
    std::println(csv, "{},{},{},{},{},{},{},{},{},{}", snapshot.filename,
                 snapshot.total_time.count(), snapshot.batch_time.count(), snapshot.samples,
                 snapshot.batch_size, snapshot.error.error, snapshot.error.absolute,
                 snapshot.error.squared, snapshot.error.relative_absolute,
                 snapshot.error.relative_squared);
  }

  LOG_INFO("Rendering complete, wrote data to output/box/{}.csv", TRM_VERSION_COMMIT);

  return 0;
}
