#ifndef INCLUDE_SRC_SCENE_HPP_
#define INCLUDE_SRC_SCENE_HPP_

#include "render.hpp"
#include "types.hpp"

namespace trm {
  extern Material mats[];

  std::tuple<Float, std::uint32_t> sdf(Vec3 p);
  Vec3 norm(Vec3 p, std::uint32_t obj);
} // namespace trm

#endif // INCLUDE_SRC_SCENE_HPP_
