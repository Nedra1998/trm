#ifndef INCLUDE_SRC_TYPES_HPP_
#define INCLUDE_SRC_TYPES_HPP_

#define GLM_FORCE_SWIZZLE
#define GLM_LEFT_HANDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

namespace trm {
#ifdef TRM_DOUBLE_PRECISION
  typedef glm::dvec2 Vec2;
  typedef glm::dvec3 Vec3;
  typedef glm::dvec4 Vec4;
  typedef glm::dmat4 Mat4;
  typedef double Float;
#else
  typedef glm::vec2 Vec2;
  typedef glm::vec3 Vec3;
  typedef glm::vec4 Vec4;
  typedef glm::mat4 Mat4;
  typedef float Float;
#endif

  enum Shading { EMISSIVE, DIFFUSE, SPECULAR, REFRACTIVE, FRESNEL };
  struct Material {
    Shading shading;
    Vec3 color;
    Float emission, ior;
  };

  struct Ray {
    Ray(const Vec3 &o, const Vec3 &d, const std::uint32_t mat = 0)
        : o(o), d(normalize(d)), material(mat) {}
    Vec3 o, d;
    std::uint32_t material;
  };

  struct Camera {
    Vec3 pos, center, up;
    Float fov;
  };

} // namespace trm

#endif // INCLUDE_SRC_TYPES_HPP_
