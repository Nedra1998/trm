#include "render.hpp"

#include <chrono>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "log.hpp"
#include "rand.hpp"
#include "scene.hpp"

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

double trm::Renderer::render(Image &img, std::uint32_t samples) {
  ZoneScoped;
  const auto start = std::chrono::high_resolution_clock::now();

  Mat4 view = inverse(lookAtLH(camera.pos, camera.center, camera.up));
  Float filmz = img.width / (2.0f * tan(0.5f * camera.fov));
  Vec3 origin = view * Vec4(0.0f, 0.0f, 0.0f, 1.0f);

  for (std::size_t i = 0; i < img.width * img.height; ++i) {
    std::size_t x = i % img.width;
    std::size_t y = i / img.width;

    if (x == 0) {
      FrameMark;
      LOG_TRACE("Rendering pixel ({},{})", x, y);
    }

    PixelTraceResult pixel;
    {
      ZoneScopedN("render_pixel");
      for (std::uint32_t s = 0; s < samples; ++s) {
        pixel += trace(Ray(origin, view * Vec4(x - img.width / 2.0f + frand(),
                                               y - img.height / 2.0f + frand(), filmz, 0.0f)));
      }
    }

    Float weight = (static_cast<Float>(img.samples) / static_cast<Float>(img.samples + samples));
    PixelTraceResult raw = pixel;
    pixel *= (1.0f / static_cast<Float>(img.samples + samples));
    img.color[i * 3 + 0] = (weight * img.color[i * 3 + 0]) + pixel.color.r;
    img.color[i * 3 + 1] = (weight * img.color[i * 3 + 1]) + pixel.color.g;
    img.color[i * 3 + 2] = (weight * img.color[i * 3 + 2]) + pixel.color.b;
    img.albedo[i * 3 + 0] = (weight * img.albedo[i * 3 + 0]) + pixel.albedo.r;
    img.albedo[i * 3 + 1] = (weight * img.albedo[i * 3 + 1]) + pixel.albedo.g;
    img.albedo[i * 3 + 2] = (weight * img.albedo[i * 3 + 2]) + pixel.albedo.b;
    img.normal[i * 3 + 0] = (weight * img.normal[i * 3 + 0]) + pixel.normal.r;
    img.normal[i * 3 + 1] = (weight * img.normal[i * 3 + 1]) + pixel.normal.g;
    img.normal[i * 3 + 2] = (weight * img.normal[i * 3 + 2]) + pixel.normal.b;
    img.depth[i] = (weight * img.depth[i]) + pixel.depth;
  }
  img.samples += samples;
  return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count();
}

trm::Renderer::PixelTraceResult trm::Renderer::trace(const Ray &r, const std::size_t depth) {
  ZoneScoped;
  Float rr_weight = 1.0;
  if (depth >= max_depth) {
    if (features & Features::RUSSIAN_ROULETTE && frand() > russian_roulette) {
      rr_weight = 1.0f / (1.0f - russian_roulette);
    } else {
      return {};
    }
    if (depth >= 2 * max_depth)
      return {};
  }

  const auto [t, id] = ray_march(r);
  if (id == 0) {
    return {};
  }

  Vec3 p = r.o + r.d * t;
  Vec3 normal = norm(p, id - 1);
  Mat material = mats[id - 1];

  switch (material.shading) {
    case EMISSIVE:
      {
        return {material.emission * material.color, material.color, normal, t};
        break;
      }

    case DIFFUSE:
      {
        Vec3 rotx, roty;
        ons(normal, rotx, roty);
        Vec3 sampled_dir = hemisphere(frand(), frand());
        Vec3 rotated_dir(dot(Vec3(rotx.x, roty.x, normal.x), sampled_dir),
                         dot(Vec3(rotx.y, roty.y, normal.y), sampled_dir),
                         dot(Vec3(rotx.z, roty.z, normal.z), sampled_dir));
        Float cost = dot(rotated_dir, normal);
        const auto result = trace(Ray(p + (bias * rotated_dir), rotated_dir), depth + 1);
        return {result.color * cost * material.color * rr_weight * 0.25f, material.color, normal,
                t};
        break;
      }

    case SPECULAR:
      {
        Vec3 reflected = normalize(reflect(r.d, normal));
        const auto result = trace(Ray(p + (bias * reflected), reflected), depth + 1);
        return {result.color, result.albedo, normal, t};
        break;
      }

    case REFRACTIVE:
      {
        Vec3 refracted = refract(r.d, (r.material == id) ? -normal : normal,
                                 r.material != 0 ? (mats[r.material - 1].ior / material.ior)
                                                 : (1.0f / material.ior));
        const auto result = trace(
            Ray(p + (bias * refracted), refracted, r.material == 0 || r.material != id ? id : 0),
            depth + 1);
        return {result.color * rr_weight * 1.15f, Vec3(1.0f), normal, t};
      }
  }

  return {};
}

std::tuple<trm::Float, std::uint32_t> trm::Renderer::ray_march(const Ray &r) {
  ZoneScoped;
  Float delta = 0.0;
  std::uint32_t obj = 0;
  for (Float t = 0.0; t < max_dist; t += delta) {
    std::tie(delta, obj) = sdf(r.o + r.d * t);
    if (delta < epsilon) {
      return {t + delta, obj};
    }
  }
  return {max_dist, 0};
}
