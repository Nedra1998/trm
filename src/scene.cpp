#include "scene.hpp"

#include <functional>
#include <limits>
#include <tuple>

#include <glm/gtc/matrix_transform.hpp>

#include "render.hpp"

using namespace trm;
using namespace glm;

// clang-format off
static trm::Mat4 transforms[] = {
    Mat4(1.0f),
    Mat4(1.0f),
    Mat4(1.0f),
    Mat4(1.0f),
    Mat4(1.0f),
    Mat4(1.0f),
    translate(Mat4(1.0f), Vec3(4.0f, -0.5f, 4.0f)),
    translate(Mat4(1.0f), Vec3(0.5, -2.0, 3.0)),
    translate(rotate(Mat4(1.0f), -0.785375f, Vec3(1.0, 0.0, -1.0)), Vec3(-3.0, 0.0, 3.0))
};

static trm::Mat4 inverses[] = {
    Mat4(1.0f),
    Mat4(1.0f),
    Mat4(1.0f),
    Mat4(1.0f),
    Mat4(1.0f),
    Mat4(1.0f),
    translate(Mat4(1.0f), -Vec3(4.0f, -0.5f, 4.0f)),
    translate(Mat4(1.0f), -Vec3(-0.5, -2.0, 3.0)),
    translate(rotate(Mat4(1.0f), 0.785375f, Vec3(1.0, 0.0, -1.0)), -Vec3(-3.0, 0.0, 3.0))
};

static std::function<Float(Vec3)> objects[] = {
    [](Vec3 p) { return dot(p, Vec3(0.0f, 1.0f, 0.0f)) - -3.5f; },   // Floor
    [](Vec3 p) { return dot(p, Vec3(0.0f, -1.0f, 0.0f)) - -10.5f; }, // Ceiling
    [](Vec3 p) { return dot(p, Vec3(1.0f, 0.0f, 0.0f)) - -7.0f; },   // Right
    [](Vec3 p) { return dot(p, Vec3(-1.0f, 0.0f, 0.0f)) - -7.0f; },  // Left
    [](Vec3 p) { return dot(p, Vec3(0.0f, 0.0f, 1.0f)) - -7.0f; },   // Front
    [](Vec3 p) { return dot(p, Vec3(0.0f, 0.0f, -1.0f)) - -7.0f; },  // Back
    [](Vec3 p) { return length(p) - 3.0f; },
    // [](Vec3 p) {
    //   Vec3 q = abs(p) - Vec3(1.0f, 1.0f, 1.0f);
    //   return length(max(q, 0.0f)) + min(max(q.x, max(q.y, q.z)), 0.0f);
    // },
    [](Vec3 p) { return length(p) - 1.5f; },
    [](Vec3 p) { return length(Vec2(length(p.xz()) - 3.0f, p.y)) - 0.75f; }
};

trm::Material trm::mats[] = {
    {DIFFUSE,    Vec3(1.0f,    1.0f,    1.0f),    0.0f,  0.0f}, // Floor
    {EMISSIVE,   Vec3(1.0f,    1.0f,    1.0f),    10.0f, 0.0f}, // Ceiling
    {DIFFUSE,    Vec3(0.12941, 0.58823, 0.95294), 0.0f,  0.0f}, // Right
    {DIFFUSE,    Vec3(0.95686, 0.26274, 0.21176), 0.0f,  0.0f}, // Left
    {DIFFUSE,    Vec3(0.0f,    0.0f,    1.0f),    0.0f,  0.0f}, // Front
    {DIFFUSE,    Vec3(0.29803, 0.68627, 0.31372), 0.0f,  0.0f}, // Back
    {DIFFUSE,    Vec3(1.0f,    1.0f,    1.0f),    0.0f,  0.0f},
    {SPECULAR,   Vec3(1.0f,    1.0f,    1.0f),    0.0f,  0.0f},
    {FRESNEL, Vec3(1.0f,    1.0f,    1.0f),    0.0f,  1.5f},
};
// clang-format on

std::tuple<trm::Float, std::uint32_t> trm::sdf(Vec3 p) {
  Float dist = std::numeric_limits<Float>::infinity();
  std::uint32_t obj = 0;

  for (std::size_t i = 0; i < sizeof(objects) / sizeof(objects[0]); ++i) {
    Float obj_dist = abs(objects[i](inverses[i] * Vec4(p, 1.0f)));
    if (obj_dist < dist) {
      dist = obj_dist;
      obj = i + 1;
    }
  }

  return {dist, obj};
}

Vec3 trm::norm(Vec3 p, std::uint32_t obj) {
  const auto &func = objects[obj];
  Vec3 op = inverses[obj] * Vec4(p, 1.0f);
  return normalize(
      transforms[obj] *
      Vec4(func(Vec3(op.x + 0.001f, op.y, op.z)) - func(Vec3(op.x - 0.001f, op.y, op.z)),
           func(Vec3(op.x, op.y + 0.001f, op.z)) - func(Vec3(op.x, op.y - 0.001f, op.z)),
           func(Vec3(op.x, op.y, op.z + 0.001f)) - func(Vec3(op.x, op.y, op.z - 0.001f)), 0.0f));
}
