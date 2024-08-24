#ifndef TRM_MATERIAL_HPP_
#define TRM_MATERIAL_HPP_

#include <cstdint>
#include <string>

#include "trm/float.hpp"

namespace trm {
  enum struct Shading : std::uint8_t { EMISSIVE, DIFFUSE, SPECULAR, REFRACTIVE, FRESNEL };

  inline Vec3 Hex(const std::string &hex) {
    const auto r = std::stoul(hex.substr(1, 2), nullptr, 16) / 255.0f;
    const auto g = std::stoul(hex.substr(3, 2), nullptr, 16) / 255.0f;
    const auto b = std::stoul(hex.substr(5, 2), nullptr, 16) / 255.0f;
    return Vec3(r, g, b);
  }

  struct Material {
    Shading shading;
    Vec3 color;
    Float emission, ior = 1.0f;
  };
} // namespace trm

#endif // TRM_MATERIAL_HPP_
