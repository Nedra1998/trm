/**
 * @file float.hpp
 * @brief TRM floating point type
 *
 * This header file defines the floating point type used by TRM. It is configured as a pre-processor
 * macro set by the CMake configuration. If `TRM_DOUBLE_PRECISION` is defined, the floating point
 * type is `double`, otherwise it is `float`.
 */

#ifndef TRM_FLOAT_HPP_
#define TRM_FLOAT_HPP_

#include <glm/glm.hpp>

namespace trm {
#ifdef TRM_DOUBLE_PRECISION
  typedef double Float;
  typedef glm::dvec2 Vec2;
  typedef glm::dvec3 Vec3;
  typedef glm::dvec4 Vec4;
  typedef glm::dmat4 Mat4;
#else
  typedef float Float;
  typedef glm::vec2 Vec2;
  typedef glm::vec3 Vec3;
  typedef glm::vec4 Vec4;
  typedef glm::mat4 Mat4;
#endif
} // namespace trm

#endif // TRM_FLOAT_HPP_
