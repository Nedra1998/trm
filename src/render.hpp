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

  struct Camera {
    Vec3 pos, center, up;
    Float fov;
  };

  enum Features : std::uint32_t {
    NONE = 0,
    RUSSIAN_ROULETTE = 1 << 0,
  };

  class Renderer {
  public:
    std::uint32_t max_depth = 16;
    Float epsilon = 1e-5f;
    Float max_dist = 1e5f;
    Float bias = 10.0f * epsilon;

    Features features = Features::NONE;
    Float russian_roulette = 0.1f;

    Camera camera = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, M_PI / 2.0f};

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

  Image render(std::string_view pattern, bool progressive = false);
  std::tuple<Vec3, Vec3, Vec3, Float> trace(const Ray &r, const std::size_t depth = 0);
  std::tuple<Float, std::uint32_t> ray_march(const Ray &r);

  void ons(const Vec3 &v1, Vec3 &v2, Vec3 &v3);
  Vec3 hemisphere(const Float &u1, const Float &u2);
} // namespace trm

#endif // INCLUDE_SRC_RENDER_HPP_
