#include "material.hpp"
#define GLM_LEFT_HANDED

#include <chrono>

#include <glm/gtc/matrix_transform.hpp>

#include "log.hpp"
#include "rand.hpp"
#include "renderer.hpp"

void construct_basis(const trm::Vec3 &v1, trm::Vec3 &v2, trm::Vec3 &v3) {
  if (fabs(v1.x) > fabs(v1.y)) {
    trm::Float inv_len = 1.0 / sqrt(v1.x * v1.x + v1.z * v1.z);
    v2 = trm::Vec3(-v1.z * inv_len, 0.0, v1.x * inv_len);
  } else {
    trm::Float inv_len = 1.0 / sqrt(v1.y * v1.y + v1.z * v1.z);
    v2 = trm::Vec3(0.0, v1.z * inv_len, -v1.y * inv_len);
  }
  v3 = cross(v1, v2);
}

trm::Vec3 sample_hemisphere(const trm::Float &u1, const trm::Float &u2) {
  const trm::Float r = sqrt(1.0 - u1 * u1);
  const trm::Float phi = 2 * M_PI * u2;
  return trm::Vec3(cos(phi) * r, sin(phi) * r, u1);
}

std::chrono::milliseconds trm::Renderer::render(ImageSet &img, std::uint32_t samples) {
  PROFILE_ZONE();
  const auto start = std::chrono::high_resolution_clock::now();

  Mat4 view = inverse(lookAtLH(camera.pos, camera.center, camera.up));
  Float filmz = img.width / (2.0f * tan(0.5f * camera.fov));
  Vec3 origin = view * Vec4(0.0f, 0.0f, 0.0f, 1.0f);

  Float image_weight =
      (static_cast<Float>(img.samples) / static_cast<Float>(img.samples + samples));
  if (img.samples != 0) {
    img *= image_weight;
  }

  for (std::size_t i = 0; i < img.width * img.height; ++i) {
    std::size_t x = i % img.width;
    std::size_t y = i / img.width;

    Pixel pixel;
    for (std::uint32_t s = 0; s < samples; ++s) {
      pixel += trace(Ray(origin, view * Vec4(x - img.width / 2.0f + frand(),
                                             y - img.height / 2.0f + frand(), filmz, 0.0f)));
    }
    pixel *= (1.0f / static_cast<Float>(img.samples + samples));

    img.color.set(i, pixel.color);
    img.albedo.set(i, pixel.albedo);
    img.normal.set(i, pixel.normal);
    img.depth.set(i, pixel.depth);
  }

  img.samples += samples;

  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - start);
}

trm::Renderer::Pixel trm::Renderer::trace(const Ray &r, const std::size_t depth) {
  PROFILE_ZONE();

  if (depth >= cfg.max_depth) {
    return Pixel();
  }

  const auto [t, obj] = ray_march(r);
  if (obj == 0) {
    return Pixel();
  }

  Vec3 p = r.o + r.d * t;
  Vec3 normal = (r.material == obj) ? -scene.normal(p, obj) : scene.normal(p, obj);
  Material mat = scene.materials(obj);

  switch (mat.shading) {
    case Shading::EMISSIVE:
      {
        return Pixel{mat.emission * mat.color, mat.color, normal, t};
        break;
      }
    case Shading::DIFFUSE:
      {
        Vec3 xbasis, ybasis;
        construct_basis(normal, xbasis, ybasis);
        Vec3 sample = sample_hemisphere(frand(), frand());
        Vec3 dir(dot(Vec3(xbasis.x, ybasis.x, normal.x), sample),
                 dot(Vec3(xbasis.y, ybasis.y, normal.y), sample),
                 dot(Vec3(xbasis.z, ybasis.z, normal.z), sample));
        const auto pixel = trace(Ray(p + (cfg.bias * dir), dir), depth + 1);
        return Pixel{.color = pixel.color * mat.color * dot(normal, dir) / Float(M_PI),
                     .albedo = mat.color,
                     .normal = normal,
                     .depth = t};
        break;
      }
    case Shading::SPECULAR:
      {
        Vec3 reflected = reflect(r.d, normal);
        const auto pixel = trace(Ray(p + (cfg.bias * reflected), reflected), depth + 1);
        return Pixel{.color = pixel.color, .albedo = pixel.color, .normal = normal, .depth = t};
        break;
      }
    case Shading::REFRACTIVE:
      {
        Float ior = (r.material == obj)
                        ? (mat.ior / 1.0f)
                        : ((r.material != 0) ? (scene.materials(r.material).ior / mat.ior)
                                             : (1.0f / mat.ior));
        Vec3 refracted = refract(r.d, normal, ior);
        const auto pixel = trace(Ray(p + (cfg.bias * refracted), refracted,
                                     (r.material == 0 || r.material != obj) ? obj : 0),
                                 depth + 1);
        return Pixel{.color = pixel.color, .albedo = Vec3(1.0f), .normal = normal, .depth = t};
      }
    case Shading::FRESNEL:
      {
        Float cior = (r.material != 0) ? scene.materials(r.material).ior : 1.0f;
        Float cos_theta = dot(-r.d, normal);
        Float r0 = (cior - mat.ior) / (cior + mat.ior);
        r0 = r0 * r0;
        Float fresnel = r0 + (1.0f - r0) * pow(1.0f - cos_theta, 5);

        if (frand() < fresnel) {
          Vec3 reflected = normalize(reflect(r.d, normal));
          const auto pixel = trace(Ray(p + (cfg.bias * reflected), reflected), depth + 1);
          return Pixel{.color = pixel.color, .albedo = pixel.color, .normal = normal, .depth = t};
        } else {
          Vec3 refracted = refract(r.d, (r.material == obj) ? -normal : normal, cior / mat.ior);
          const auto pixel = trace(Ray(p + (cfg.bias * refracted), refracted,
                                       (r.material == 0 || r.material != obj) ? obj : 0),
                                   depth + 1);
          return Pixel{.color = pixel.color, .albedo = Vec3(1.0f), .normal = normal, .depth = t};
        }
      }
  }

  return Pixel();
}

std::tuple<trm::Float, std::uint32_t> trm::Renderer::ray_march(const Ray &r) {
  ZoneScoped;
  Float delta = 0.0;
  std::uint32_t obj = 0;
  std::int32_t steps = 0;
  for (Float t = 0.0; t < cfg.max_distance && steps < cfg.max_steps; t += delta) {
    std::tie(delta, obj) = scene.sdf(r.o + r.d * t);
    if (delta < cfg.epsilon) {
      return {t + delta, obj};
    }
    ++steps;
  }
  return {cfg.max_distance, 0};
}
