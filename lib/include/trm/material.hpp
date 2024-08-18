#ifndef TRM_MATERIAL_HPP_
#define TRM_MATERIAL_HPP_

#include <cstdint>

#include "trm/float.hpp"

namespace trm {
  enum struct Shading : std::uint8_t { EMISSIVE, DIFFUSE, SPECULAR, REFRACTIVE, FRESNEL };

  struct Material {
    Shading shading;
    Vec3 color;
    Float emission, ior = 1.0f;
  };
} // namespace trm

#endif // TRM_MATERIAL_HPP_
