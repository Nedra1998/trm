#include <glm/gtc/matrix_transform.hpp>
#include <trm/trm.hpp>

#include "example.hpp"

using namespace trm;
using namespace glm;

Camera CAMERA = Camera{
    .pos = {0.0f, 0.0f, -1.33333f},
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
            scale(Mat4(1.0f), Vec3(1.0)),
        },
    .inv_transforms_ =
        {
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            Mat4(1.0f),
            scale(Mat4(1.0f), 1.0f / Vec3(1.0)),
        },
    .materials_ =
        {
            {Shading::EMISSIVE, Hex("#FFFFFF"), 35.0f, 0.0f},
            {Shading::EMISSIVE, Hex("#F44336"), 25.0f, 0.0f}, // Front
            {Shading::EMISSIVE, Hex("#4CAF50"), 25.0f, 0.0f}, // Right
            {Shading::EMISSIVE, Hex("#2196F3"), 25.0f, 0.0f}, // Front
            {Shading::DIFFUSE, Hex("#FFFFFF"), 0.0f, 0.0f},
        },
    .sdfs_ = {
        [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, -1.0f, 0.0f)) - -0.6666f; },  // Ceiling
        [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 1.0f, 0.0f)) - -0.66666f; },  // Floor
        [](Vec3 p) -> Float { return dot(p, Vec3(1.0f, 0.0f, 0.0f)) - -0.66666f; },  // Left
        [](Vec3 p) -> Float { return dot(p, Vec3(-1.0f, 0.0f, 0.0f)) - -0.66666f; }, // Right
        // [](Vec3 p) -> Float {
        //   Vec3 q = mod(p + 2.0f, 4.0f) - 2.0f;
        //   return length(q) - 1.0f;
        // },
        [](Vec3 p) -> Float {
          Vec3 q = abs(mod(p + 1.0f, 2.0f) - 1.0f) - Vec3(1.0f);
          Float d = length(max(q, 0.0f)) + min(max(q.x, max(q.y, q.z)), 0.0f);
          Float s = 1.0f;
          for (std::uint8_t i = 0; i < 5; ++i) {
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
