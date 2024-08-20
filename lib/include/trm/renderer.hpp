#ifndef TRM_RENDERER_HPP_
#define TRM_RENDERER_HPP_

#include <chrono>

#include "error.hpp"
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

    struct RenderStats {
      std::string filename;
      std::chrono::milliseconds total_time, batch_time;
      std::uint32_t samples, batch_size;
      Error error;
    };

    Config cfg;
    Camera camera;
    Scene scene;
    std::uint32_t target_samples = 0;

    std::vector<RenderStats> render(std::string_view filename, ImageSet &img, std::uint32_t samples,
                                    std::uint32_t batch_size, bool compute_error = true);
    std::chrono::milliseconds render(ImageSet &img, std::uint32_t samples) const;

  private:
    Pixel trace(const Ray &r, const std::size_t depth = 0) const;
    std::tuple<Float, std::uint32_t> ray_march(const Ray &r) const;
  };
} // namespace trm

#endif // TRM_RENDERER_HPP_
