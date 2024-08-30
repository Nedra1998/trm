#define GLM_FORCE_SWIZZLE

#include <glm/gtc/matrix_transform.hpp>
#include <trm/trm.hpp>

#include "example.hpp"

using namespace glm;
using namespace trm;

Camera CAMERA = Camera{
    .pos = {0.0f, 0.0f, -6.0f},
    .center = {0.0f, 0.0f, 0.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .fov = 1.5707f,
};

Scene SCENE = Scene{
    .transforms_ = {Mat4(1.0f), Mat4(1.0f), Mat4(1.0f), Mat4(1.0f), Mat4(1.0f), Mat4(1.0f),
                    translate(Mat4(1.0f), Vec3(4.0f, -0.5f, 4.0f)),
                    translate(Mat4(1.0f), Vec3(0.5, -2.0, 3.0)),
                    translate(rotate(Mat4(1.0f), -0.785375f, Vec3(1.0, 0.0, -1.0)),
                              Vec3(-3.0, 0.0, 3.0))},
    .inv_transforms_ = {Mat4(1.0f), Mat4(1.0f), Mat4(1.0f), Mat4(1.0f), Mat4(1.0f), Mat4(1.0f),
                        translate(Mat4(1.0f), -Vec3(4.0f, -0.5f, 4.0f)),
                        translate(Mat4(1.0f), -Vec3(-0.5, -2.0, 3.0)),
                        translate(rotate(Mat4(1.0f), 0.785375f, Vec3(1.0, 0.0, -1.0)),
                                  -Vec3(-3.0, 0.0, 3.0))},
    .materials_ =
        {
            {Shading::DIFFUSE, Vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f},   // Floor
            {Shading::EMISSIVE, Vec3(1.0f, 1.0f, 1.0f), 10.0f, 0.0f}, // Ceiling
            {Shading::DIFFUSE, Hex("#F44336"), 0.0f, 0.0f},           // Front
            {Shading::DIFFUSE, Hex("#4CAF50"), 0.0f, 0.0f},           // Right
            {Shading::DIFFUSE, Hex("#FFEB3B"), 0.0f, 0.0f},           // Back
            {Shading::DIFFUSE, Hex("#2196F3"), 0.0f, 0.0f},           // Front
            {Shading::DIFFUSE, Vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f},
            {Shading::SPECULAR, Vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f},
            {Shading::FRESNEL, Vec3(1.0f, 1.0f, 1.0f), 0.0f, 1.5f},
        },
    .sdfs_ = {[](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 1.0f, 0.0f)) - -3.5f; },   // Floor
              [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, -1.0f, 0.0f)) - -10.5f; }, // Ceiling
              [](Vec3 p) -> Float { return dot(p, Vec3(1.0f, 0.0f, 0.0f)) - -7.0f; },   // Left
              [](Vec3 p) -> Float { return dot(p, Vec3(-1.0f, 0.0f, 0.0f)) - -7.0f; },  // Right
              [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, 1.0f)) - -7.0f; },   // Back
              [](Vec3 p) -> Float { return dot(p, Vec3(0.0f, 0.0f, -1.0f)) - -7.0f; },  // Front
              [](Vec3 p) -> Float { return length(p) - 3.0f; },
              [](Vec3 p) -> Float { return length(p) - 1.5f; },
              [](Vec3 p) -> Float { return length(Vec2(length(p.xz()) - 3.0f, p.y)) - 0.75f; }},
};
