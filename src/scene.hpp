#ifndef INCLUDE_SRC_SCENE_HPP_
#define INCLUDE_SRC_SCENE_HPP_

#include "types.hpp"

namespace trm {
  enum Shading { EMISSIVE, DIFFUSE, SPECULAR, REFRACTIVE };
  struct Mat {
    Shading shading;
    Vec3 color;
    Float emission, roughness, ior;
  };

  extern Mat mats[];

  std::tuple<Float, std::uint32_t> sdf(Vec3 p);
  Vec3 norm(Vec3 p, std::uint32_t obj);
} // namespace trm

#endif // INCLUDE_SRC_SCENE_HPP_
