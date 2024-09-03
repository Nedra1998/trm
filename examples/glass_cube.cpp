#define GLM_FORCE_SWIZZLE
#define GLM_LEFT_HANDED
#include <glm/gtc/matrix_transform.hpp>
#include <trm/trm.hpp>

#include "example.hpp"

using namespace trm;
using namespace glm;

Float dot2(Vec3 a) { return dot(a, a); }

Camera CAMERA = Camera{
    .pos = {-4.0f, 3.65f, -4.0f},
    .center = {0.0f, 1.25f, 0.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .fov = 0.69f,
};

Scene SCENE = Scene{
    .sky = Vec3(0.8f),
    .transforms_ =
        {
            Mat4(1.0f),
            translate(Mat4(1.0f), Vec3(0.0f, 1.25f, 0.0f)),
            translate(rotate(Mat4(1.0f), 1.570796f, Vec3(1.0f, 0.0f, 0.0f)),
                      Vec3(0.0f, 1.25f, 0.0f)),
            translate(rotate(Mat4(1.0f), 0.7853982f, Vec3(0.5f, 0.0f, 0.5f)),
                      Vec3(3.5f, 3.5f, -3.5f)),
        },
    .inv_transforms_ =
        {
            Mat4(1.0f),
            translate(Mat4(1.0f), -Vec3(0.0f, 1.25f, 0.0f)),
            translate(rotate(Mat4(1.0f), -1.570796f, Vec3(1.0f, 0.0f, 0.0f)),
                      -Vec3(0.0f, 1.25f, 0.0f)),
            translate(rotate(Mat4(1.0f), -0.7853982f, Vec3(0.5f, 0.0f, 0.5f)),
                      -Vec3(3.5f, 3.5f, -3.5f)),
        },
    .materials_ =
        {
            {Shading::DIFFUSE, Hex("#E7E7E7"), 0.0f, 0.0f},
            {Shading::FRESNEL, Hex("#FFFFFF"), 0.0f, 1.333f},
            {Shading::DIFFUSE, Hex("#FFEB3B"), 0.0f, 0.0f},
            {Shading::EMISSIVE, Hex("#FFFFFF"), 20.0f, 0.0f}, // Light
        },
    .sdfs_ = {
        [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 1.0f, 0.0f)) - -0.0f; }, // Floor
        [](Vec3 p) -> Float {
          vec3 q = abs(p) - Vec3(1.0f, 1.0f, 1.0f);
          return length(max(q, 0.0f)) + min(max(q.x, max(q.y, q.z)), 0.0f);
        },
        [](Vec3 p) -> Float { return length(Vec2(length(p.xz()) - 0.75f, p.y)) - 0.25f; },
        [](Vec3 p) -> Float {
          vec3 q = abs(p) - Vec3(2.5f, 2.5f, 1.0f);
          return length(max(q, 0.0f)) + min(max(q.x, max(q.y, q.z)), 0.0f);
        },
        // [](Vec3 p) -> Float { return length(p) - 1.0f; },
    },
};
