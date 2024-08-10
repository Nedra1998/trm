#include "render.hpp"

#include <chrono>

#include <glm/gtc/matrix_transform.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <indicators/setting.hpp>
#include <spdlog/spdlog.h>

#include "rand.hpp"
#include "scene.hpp"

using namespace indicators;

void trm::ons(const Vec3 &v1, Vec3 &v2, Vec3 &v3) {
  if (abs(v1.x) > abs(v1.y)) {
    Float inv_len = 1.0 / sqrt(v1.x * v1.x + v1.z * v1.z);
    v2 = Vec3(-v1.z * inv_len, 0.0, v1.x * inv_len);
  } else {
    Float inv_len = 1.0 / sqrt(v1.y * v1.y + v1.z * v1.z);
    v2 = Vec3(0.0, v1.z * inv_len, -v1.y * inv_len);
  }
  v3 = cross(v1, v2);
}

trm::Vec3 trm::hemisphere(const Float &u1, const Float &u2) {
  const Float r = sqrt(1.0 - u1 * u1);
  const Float phi = 2 * M_PI * u2;
  return Vec3(cos(phi) * r, sin(phi) * r, u1);
}

trm::Image trm::render(std::string_view pattern, bool progressive) {
  std::size_t width = 512, height = 512, samples = 256;
  Image img(width, height);

  Mat4 view = inverse(lookAtLH(Vec3(0.0f, 0.0f, -6.0f), Vec3(0.0f), Vec3(0.0f, 1.0f, 0.0f)));
  Float filmz = width / (2.0f * tan(0.5f * 1.5707f));
  Vec3 origin = view * Vec4(0.0f, 0.0f, 0.0f, 1.0f);

  std::chrono::time_point<std::chrono::high_resolution_clock> start =
      std::chrono::high_resolution_clock::now();

  if (progressive) {
    for (std::size_t s = 0; s < samples; ++s) {
      for (std::size_t i = 0; i < width * height; ++i) {
        std::size_t x = i % width;
        std::size_t y = i / width;

        if ((s * width * height + i) % 10000 == 0) {
          float progress = (float)(s * width * height + i) / (float)(samples * width * height);
          const auto diff = std::chrono::high_resolution_clock::now() - start;
          const float elapsed = std::chrono::duration<float>(diff).count();
          const float remaining = elapsed / progress - elapsed;
          std::string bar = std::format(
              "[{:40}]", std::string((int)(40.0f * progress), '=') + (progress < 1.0f ? ">" : "") +
                             std::string(40 - (int)(40.0f * progress), ' '));
          SPDLOG_INFO("{} {} {:3.0f}% [{:02}:{:02}<{:02}:{:02}]", pattern, bar, progress * 100.0f,
                      (int)elapsed / 60, (int)elapsed % 60, (int)remaining / 60,
                      (int)remaining % 60);
        }

        Ray ray(origin,
                view * Vec4(x - width / 2.0f + frand(), y - height / 2.0f + frand(), filmz, 0.0f));
        const auto [c, a, n, d] = trace(ray);

        Float adjust = ((Float)s + 1.0f) / ((Float)s + 2.0f);
        Float demon = 1.0f / ((Float)s + 1.0f);
        img.color[i * 3 + 0] = img.color[i * 3 + 0] * adjust + c.r * demon;
        img.color[i * 3 + 1] = img.color[i * 3 + 1] * adjust + c.g * demon;
        img.color[i * 3 + 2] = img.color[i * 3 + 2] * adjust + c.b * demon;

        img.albido[i * 3 + 0] = img.albido[i * 3 + 0] * adjust + a.r * demon;
        img.albido[i * 3 + 1] = img.albido[i * 3 + 1] * adjust + a.g * demon;
        img.albido[i * 3 + 2] = img.albido[i * 3 + 2] * adjust + a.b * demon;

        img.normal[i * 3 + 0] = img.normal[i * 3 + 0] * adjust + n.r * demon;
        img.normal[i * 3 + 1] = img.normal[i * 3 + 1] * adjust + n.g * demon;
        img.normal[i * 3 + 2] = img.normal[i * 3 + 2] * adjust + n.b * demon;

        img.depth[i] = img.depth[i] * adjust + d * demon;
      }

      img.write(std::format("{}color.{:03d}.png", pattern, s), Buffer::COLOR);
    }
  } else {
    for (std::size_t i = 0; i < width * height; ++i) {
      std::size_t x = i % width;
      std::size_t y = i / width;

      Vec3 color(0.0f, 0.0f, 0.0f);
      Vec3 albedo(0.0f, 0.0f, 0.0f);
      Vec3 normal(0.0f, 0.0f, 0.0f);
      Float depth = 0.0f;

      for (std::size_t s = 0; s < samples; ++s) {
        Ray ray(origin,
                view * Vec4(x - width / 2.0f + frand(), y - height / 2.0f + frand(), filmz, 0.0f));
        const auto [c, a, n, d] = trace(ray);
        color += c / (Float)samples;
        albedo += a / (Float)samples;
        normal += n / (Float)samples;
        depth += d / (Float)samples;
      }

      img.color[i * 3 + 0] = clamp(color.r, 0.0f, 1.0f);
      img.color[i * 3 + 1] = clamp(color.g, 0.0f, 1.0f);
      img.color[i * 3 + 2] = clamp(color.b, 0.0f, 1.0f);

      img.albido[i * 3 + 0] = clamp(albedo.r, 0.0f, 1.0f);
      img.albido[i * 3 + 1] = clamp(albedo.g, 0.0f, 1.0f);
      img.albido[i * 3 + 2] = clamp(albedo.b, 0.0f, 1.0f);

      img.normal[i * 3 + 0] = clamp(normal.x, -1.0f, 1.0f);
      img.normal[i * 3 + 1] = clamp(normal.y, -1.0f, 1.0f);
      img.normal[i * 3 + 2] = clamp(normal.z, -1.0f, 1.0f);

      img.depth[i] = depth;
    }
  }

  return img;
}

std::tuple<trm::Vec3, trm::Vec3, trm::Vec3, trm::Float> trm::trace(const Ray &r,
                                                                   std::size_t depth) {
  Float rr_factor = 1.0;
  if (depth >= 32) {
    if (frand() <= 0.1f)
      return {Vec3(0.0f), Vec3(0.0f), Vec3(0.0f), 0.0f};
    rr_factor = 1.0 / (1.0 - 0.1);
  }
  const auto [t, id] = ray_march(r);
  if (id == 0) {
    return {Vec3(0.0f), Vec3(0.0f), Vec3(0.0f), 100.0f};
  } else {
    Vec3 p = r.o + r.d * t;
    Vec3 normal = norm(p, id - 1);
    Vec3 color(0.0f);

    Mat mat = mats[id - 1];
    switch (mat.shading) {
    case EMISSIVE: {
      color = mat.emission * mat.color;
      break;
    }
    case DIFFUSE: {
      Vec3 rotx, roty;
      ons(normal, rotx, roty);
      Vec3 sampled_dir = hemisphere(trm::frand(), trm::frand());
      Vec3 rotated_dir;
      rotated_dir.x = dot(Vec3(rotx.x, roty.x, normal.x), sampled_dir);
      rotated_dir.y = dot(Vec3(rotx.y, roty.y, normal.y), sampled_dir);
      rotated_dir.z = dot(Vec3(rotx.z, roty.z, normal.z), sampled_dir);
      Float cost = dot(rotated_dir, normal);
      const auto [c, a, n, d] = trace(Ray(p + (0.001f * rotated_dir), rotated_dir), depth + 1);
      color += c * cost * mat.color * rr_factor * 0.25f;
      break;
    }
    case SPECULAR: {
      Vec3 reflected = normalize(reflect(r.d, normal));
      const auto [c, a, n, d] = trace(Ray(p + (0.001f * reflected), reflected), depth + 1);
      color += c;
      break;
    }
    case REFRACTIVE:
      Vec3 refracted =
          refract(r.d, (r.material == id) ? -normal : normal,
                  r.material != 0 ? (mats[r.material - 1].ior / mat.ior) : (1.0f / mat.ior));
      const auto [c, a, n, d] = trace(
          Ray(p + (0.01f * refracted), refracted, r.material == 0 || r.material != id ? id : 0),
          depth + 1);
      color += c * rr_factor * 1.15f;
      break;
    }

    return {color, mat.shading == REFRACTIVE ? Vec3(0.0f) : mat.color, normal, t};
  }
}

std::tuple<trm::Float, std::uint32_t> trm::ray_march(const Ray &r) {
  Float delta = 0.0;
  std::uint32_t obj = 0;
  for (Float t = 0.0; t < 100.0; t += delta) {
    std::tie(delta, obj) = sdf(r.o + r.d * t);
    if (delta < 0.0001) {
      return {t + delta, obj};
    }
  }
  return {100.0f, 0};
}
