#ifndef TRM_RENDERER_HPP_
#define TRM_RENDERER_HPP_

#include <chrono>

#include "trm/config.hpp"
#include "trm/image.hpp"
#include "trm/scene.hpp"

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

  class Renderer {
  public:
    struct Pixel {
      Vec3 color = Vec3(0.0f), albedo = Vec3(0.0f), normal = Vec3(0.0f);
      Float depth = 0.0f;

      inline Pixel &operator+=(const Pixel &rhs) {
        color += rhs.color;
        albedo += rhs.albedo;
        normal += rhs.normal;
        depth += rhs.depth;
        return *this;
      }

      inline Pixel operator*=(const Float scalar) {
        color *= scalar;
        albedo *= scalar;
        normal *= scalar;
        depth *= scalar;
        return *this;
      }
    };

    Config cfg;
    Camera camera;
    Scene scene;

    std::chrono::milliseconds render(ImageSet &img, std::uint32_t samples);

  private:
    Pixel trace(const Ray &r, const std::size_t depth = 0);
    std::tuple<Float, std::uint32_t> ray_march(const Ray &r);
  };
} // namespace trm

#endif // TRM_RENDERER_HPP_
