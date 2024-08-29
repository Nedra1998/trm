/**
 * @file material.hpp
 * @brief Material utilities for TRM
 *
 * This file provides the material model for TRM, all of the interpretation of how to handle the
 * material is done within the `Renderer` class, this file only provides the data structure.
 */

#ifndef TRM_MATERIAL_HPP_
#define TRM_MATERIAL_HPP_

#include <cstdint>
#include <string>

#include "trm/float.hpp"

namespace trm {
  /**
   * @brief Material shading model to use in the renderer
   */
  enum struct Shading : std::uint8_t {
    EMISSIVE,   ///< Emissive material.
    DIFFUSE,    ///< Diffuse material using Lambertian reflection.
    SPECULAR,   ///< Specular material using perfect reflection.
    REFRACTIVE, ///< Refractive material using perfect refraction.
    FRESNEL,    ///< Refractive material using Fresnel reflection.
  };

  /**
   * @brief Construct a color from a hexadecimal string
   *
   * @param hex Color in hexadecimal format including the `#` symbol.
   * @return RGB color as a `Vec3` in the range [0, 1].
   */
  inline Vec3 Hex(const std::string &hex) {
    const auto r = std::stoul(hex.substr(1, 2), nullptr, 16) / 255.0f;
    const auto g = std::stoul(hex.substr(3, 2), nullptr, 16) / 255.0f;
    const auto b = std::stoul(hex.substr(5, 2), nullptr, 16) / 255.0f;
    return Vec3(r, g, b);
  }

  /**
   * @class Material
   * @brief The material representation for every object in the scene.
   */
  struct Material {
    Shading shading;  ///< The shading model to use for the material.
    Vec3 color;       ///< The color of the material.
    Float emission;   ///< The power of the emission of the material, only used for emission.
    Float ior = 1.0f; ///< The index of refraction of the material, only used for refraction.
  };
} // namespace trm

#endif // TRM_MATERIAL_HPP_
