#ifndef INCLUDE_SRC_RENDER_HPP_
#define INCLUDE_SRC_RENDER_HPP_

#include "img.hpp"
#include "types.hpp"

namespace trm {
  struct Ray {
    Ray(const Vec3 &o, const Vec3 &d, const std::uint32_t mat = 0)
        : o(o), d(normalize(d)), material(mat) {}
    Vec3 o, d;
    std::uint32_t material;
  };

  Image render(std::string_view pattern, bool progressive = false);
  std::tuple<Vec3, Vec3, Vec3, Float> trace(const Ray &r, std::size_t depth = 0);
  std::tuple<Float, std::uint32_t> ray_march(const Ray &r);

  void ons(const Vec3 &v1, Vec3 &v2, Vec3 &v3);
  Vec3 hemisphere(const Float &u1, const Float &u2);
} // namespace trm

#endif // INCLUDE_SRC_RENDER_HPP_
