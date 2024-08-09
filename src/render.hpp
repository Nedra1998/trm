#ifndef INCLUDE_SRC_RENDER_HPP_
#define INCLUDE_SRC_RENDER_HPP_

#include "img.hpp"
#include "types.hpp"

namespace trm {
  struct Ray {
    Ray(const Vec3 &o, const Vec3 &d) : o(normalize(o)), d(normalize(d)) {}
    Vec3 o, d;
  };

  Image render();
  std::tuple<Vec3, Vec3, Vec3, Float> trace(const Ray &r, std::size_t depth = 0);
} // namespace trm

#endif // INCLUDE_SRC_RENDER_HPP_
