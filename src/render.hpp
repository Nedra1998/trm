#ifndef INCLUDE_SRC_RENDER_HPP_
#define INCLUDE_SRC_RENDER_HPP_

#include <cstddef>
#include <functional>

#include "img.hpp"
#include "types.hpp"

namespace trm {
  enum Features : std::uint32_t {
    NONE = 0,
    PROGRESS_REPORT = 1 << 0,
    RUSSIAN_ROULETTE = 1 << 1,
  };

  class Renderer {
  public:
    std::uint32_t max_depth = 16, max_steps = 2048;
    Float epsilon = 1e-5f;
    Float max_dist = 1e5f;
    Float bias = 10.0f * epsilon;

    Features features = Features::PROGRESS_REPORT;
    Float russian_roulette = 0.1f;

    Camera camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, M_PI / 2.0f};

    std::function<void(std::size_t, std::size_t, std::size_t)> progress_callback =
        [](std::size_t, std::size_t, std::size_t) {};

    double render(Image &img, std::uint32_t samples);

  private:
    struct PixelTraceResult {
      Vec3 color = Vec3(0.0f), albedo = Vec3(0.0f), normal = Vec3(0.0f);
      Float depth = 0.0f;

      inline PixelTraceResult &operator+=(const PixelTraceResult &p) {
        color += p.color;
        albedo += p.albedo;
        normal += p.normal;
        depth += p.depth;
        return *this;
      }
      inline PixelTraceResult &operator*=(const Float &f) {
        color *= f;
        albedo *= f;
        normal *= f;
        depth *= f;
        return *this;
      }
      inline PixelTraceResult &operator/=(const Float &f) {
        Float f_inv = 1.0f / f;
        color *= f_inv;
        albedo *= f_inv;
        normal *= f_inv;
        depth *= f_inv;
        return *this;
      }
    };

    PixelTraceResult trace(const Ray &r, std::size_t depth = 0);
    std::tuple<Float, std::uint32_t> ray_march(const Ray &r);
  };

  void orthonormal_basis(const Vec3 &v1, Vec3 &v2, Vec3 &v3);
  Vec3 hemisphere(const Float &u1, const Float &u2);
} // namespace trm

#endif // INCLUDE_SRC_RENDER_HPP_
