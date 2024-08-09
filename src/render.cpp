#include "render.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "rand.hpp"

trm::Image trm::render() {
  Image img(512, 512, Buffer::COLOR | Buffer::NORMAL);

  Mat4 view = inverse(lookAtLH(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f)));
  Float filmz = 512.0f / (2.0f * tan(0.5f * 45.0f));
  Vec3 origin = view * Vec4(0.0f, 0.0f, 0.0f, 1.0f);

  for (std::size_t i = 0; i < 512 * 512; ++i) {
    std::size_t x = i % 512;
    std::size_t y = i / 512;

    Vec3 color(0.0f, 0.0f, 0.0f);
    Vec3 albedo(0.0f, 0.0f, 0.0f);
    Vec3 normal(0.0f, 0.0f, 0.0f);
    Float depth = 0.0f;

    for (std::size_t s = 0; s < 16; ++s) {
      Ray ray(origin, view * Vec4(x - 512 / 2.0f + frand(), y - 512 / 2.0f + frand(), filmz, 0.0f));
      const auto [c, a, n, d] = trace(ray);
      color += c / 16.0f;
      albedo += a / 16.0f;
      normal += n / 16.0f;
      depth += d / 16.0f;
    }

    if (!img.color.empty()) {
      img.color[i * 3 + 0] = clamp(color.r, 0.0f, 1.0f);
      img.color[i * 3 + 1] = clamp(color.g, 0.0f, 1.0f);
      img.color[i * 3 + 2] = clamp(color.b, 0.0f, 1.0f);
    }

    if (!img.albido.empty()) {
      img.albido[i * 3 + 0] = clamp(albedo.r, 0.0f, 1.0f);
      img.albido[i * 3 + 1] = clamp(albedo.g, 0.0f, 1.0f);
      img.albido[i * 3 + 2] = clamp(albedo.b, 0.0f, 1.0f);
    }

    if (!img.normal.empty()) {
      img.normal[i * 3 + 0] = clamp(normal.x, -1.0f, 1.0f);
      img.normal[i * 3 + 1] = clamp(normal.y, -1.0f, 1.0f);
      img.normal[i * 3 + 2] = clamp(normal.z, -1.0f, 1.0f);
    }

    if (!img.depth.empty()) {
      img.depth[i] = clamp(depth, 0.0f, 1.0f);
    }
  }

  return img;
}

std::tuple<trm::Vec3, trm::Vec3, trm::Vec3, trm::Float> trm::trace(const Ray &r,
                                                                   std::size_t depth) {
  return {r.d, Vec3(0.0f), r.d, 0.0f};
}
