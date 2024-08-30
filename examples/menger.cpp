#include <glm/gtc/matrix_transform.hpp>
#include <trm/trm.hpp>

#include "example.hpp"

using namespace trm;
using namespace glm;

Camera CAMERA = Camera{
    .pos = {2.0f, 4.0f, -6.0f},
    .center = {0.0f, 0.0f, 0.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .fov = 1.5707f,
};

Scene SCENE = Scene{
    .transforms_ =
        {
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            rotate(scale(Mat4(1.0f), Vec3(2.3, 2.3, 2.3)), 0.26179f, Vec3(0.0f, 1.0f, 0.0f)),
        },
    .inv_transforms_ =
        {
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            rotate(scale(Mat4(1.0f), 1.0f / Vec3(2.3, 2.3, 2.3)), -0.26179f,
                   Vec3(0.0f, 1.0f, 0.0f)),
        },
    .materials_ =
        {
            {Shading::DIFFUSE, Vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f},   // Floor
            {Shading::EMISSIVE, Vec3(1.0f, 1.0f, 1.0f), 10.0f, 0.0f}, // Ceiling
            {Shading::DIFFUSE, Hex("#F44336"), 0.0f, 0.0f},           // Front
            {Shading::DIFFUSE, Hex("#4CAF50"), 0.0f, 0.0f},           // Right
            {Shading::DIFFUSE, Hex("#FFEB3B"), 0.0f, 0.0f},           // Back
            {Shading::DIFFUSE, Hex("#2196F3"), 0.0f, 0.0f},           // Front
            {Shading::DIFFUSE, Hex("#FFFFFF"), 0.0f, 0.0f},
        },
    .sdfs_ = {
        [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 1.0f, 0.0f)) - -3.5f; },  // Floor
        [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, -1.0f, 0.0f)) - -6.0f; }, // Ceiling
        [](Vec3 p) -> Float { return dot(p, Vec3(1.0f, 0.0f, 0.0f)) - -7.0f; },  // Left
        [](Vec3 p) -> Float { return dot(p, Vec3(-1.0f, 0.0f, 0.0f)) - -7.0f; }, // Right
        [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, 1.0f)) - -7.0f; },  // Back
        [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, -1.0f)) - -7.0f; }, // Front
        [](Vec3 p) -> Float {
          Vec3 q = abs(p) - Vec3(1.0f);
          Float d = length(max(q, 0.0f)) + min(max(q.x, max(q.y, q.z)), 0.0f);
          Float s = 1.0f;
          for (std::uint8_t i = 0; i < 4; ++i) {
            Vec3 a = mod(p * s, 2.0f) - 1.0f;
            s *= 3.0f;
            Vec3 r = abs(1.0f - 3.0f * abs(a));
            Float da = max(r.x, r.y);
            Float db = max(r.y, r.z);
            Float dc = max(r.z, r.x);
            Float c = (min(da, min(db, dc)) - 1.0f) / s;
            d = max(d, c);
          }
          return d;
        },
    },
};
