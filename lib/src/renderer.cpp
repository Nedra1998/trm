#include "material.hpp"
#define GLM_LEFT_HANDED

#include <chrono>

#include <fmt/base.h>
#include <glm/gtc/matrix_transform.hpp>

#ifdef _OPENMP
#  include <omp.h>
#endif

#include "log.hpp"
#include "rand.hpp"
#include "renderer.hpp"

#ifdef _OPENMP
template <> struct fmt::formatter<omp_sched_t> : fmt::formatter<string_view> {
  auto format(omp_sched_t kind, format_context &ctx) const -> format_context::iterator {
    switch (kind) {
      case omp_sched_static:
        return fmt::format_to(ctx.out(), "static");
      case omp_sched_dynamic:
        return fmt::format_to(ctx.out(), "dynamic");
      case omp_sched_guided:
        return fmt::format_to(ctx.out(), "guided");
      case omp_sched_auto:
        return fmt::format_to(ctx.out(), "auto");
      default:
        return fmt::format_to(ctx.out(), "unknown");
    }
  }
};
#endif

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

std::vector<trm::Renderer::RenderStats> trm::Renderer::render(std::string_view filename,
                                                              ImageSet &img, std::uint32_t samples,
                                                              std::uint32_t batch_size,
                                                              Buffer buffers) {
  PROFILE_ZONE();
  std::chrono::milliseconds total_time = std::chrono::milliseconds(0);
  std::vector<RenderStats> stats;
  for (std::uint32_t s = 0; s < samples; s += batch_size) {
    std::uint32_t batch = std::min(batch_size, samples - s);
    const auto duration = render(img, batch);
    total_time += duration;

    img.write(filename, buffers);
    stats.push_back(RenderStats{
        .filename = img.fmt(filename, Buffer::COLOR),
        .total_time = total_time,
        .batch_time = duration,
        .samples = s + batch,
        .batch_size = batch,
    });

    if ((cfg.features & Feature::PROGRESS_REPORT) == Feature::PROGRESS_REPORT) {
      const float percent = static_cast<float>(s + batch) / static_cast<float>(samples);
      const std::uint32_t elapsed_hours =
          std::chrono::duration_cast<std::chrono::hours>(total_time).count();
      const std::uint32_t elapsed_minutes =
          std::chrono::duration_cast<std::chrono::minutes>(total_time).count() % 60;
      const std::uint32_t elapsed_seconds =
          std::chrono::duration_cast<std::chrono::seconds>(total_time).count() % 60;
      const auto eta = (total_time / percent) - total_time;
      const std::uint32_t eta_hours = std::chrono::duration_cast<std::chrono::hours>(eta).count();
      const std::uint32_t eta_minutes =
          std::chrono::duration_cast<std::chrono::minutes>(eta).count() % 60;
      const std::uint32_t eta_seconds =
          std::chrono::duration_cast<std::chrono::seconds>(eta).count() % 60;
      LOG_INFO("Rendered {:3d}/{:3d} samples in {:6d}ms ({:6.2f}%) "
               "[{:02d}:{:02d}:{:02d}<{:02d}:{:02d}:{:02d}]",
               s + batch, samples, duration.count(), 100.0f * percent, elapsed_hours,
               elapsed_minutes, elapsed_seconds, eta_hours, eta_minutes, eta_seconds);
    }
  }

  return stats;
}

std::chrono::milliseconds trm::Renderer::render(ImageSet &img, std::uint32_t samples) const {
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
  std::uint16_t progress = 0;

#ifdef _OPENMP
  omp_sched_t kind;
  int chunk_size;
  omp_get_schedule(&kind, &chunk_size);
  if (kind == omp_sched_dynamic && chunk_size == 1) {
    chunk_size = 1024;
    omp_set_schedule(omp_sched_dynamic, 1024);
  }
  LOG_DEBUG("OpenMP schedule kind={} chunk_size={}", kind, chunk_size);
#  pragma omp parallel for schedule(runtime) shared(img)
  for (std::size_t i = 0; i < img.width * img.height; ++i) {
#else
  for (std::size_t i = 0; i < img.width * img.height; ++i) {
#endif
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

trm::Renderer::Pixel trm::Renderer::trace(const Ray &r, const std::size_t depth) const {
  PROFILE_ZONE();

  if (depth >= cfg.max_depth) {
    return Pixel();
  }

  const auto [t, obj] = ray_march(r);
  if (obj == 0) {
    return Pixel{.color = scene.sky};
  }

  Vec3 p = r.o + r.d * t;
  Material mat = scene.materials(obj);
  Vec3 normal = (mat.shading == Shading::EMISSIVE)
                    ? Vec3(0.0f)
                    : ((r.material == obj) ? -scene.normal(p, obj) : scene.normal(p, obj));

  if (any(isnan(normal))) {
    LOG_WARN("Normal is NaN obj={} p=({}, {}, {})", obj, p.x, p.y, p.z);
    return Pixel();
  }

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
        const auto pixel = trace(Ray(p + (cfg.bias * dir), dir, r.material), depth + 1);
        return Pixel{.color = pixel.color * mat.color * dot(normal, dir) / Float(M_PI),
                     .albedo = mat.color,
                     .normal = normal,
                     .depth = t};
        break;
      }
    case Shading::SPECULAR:
      {
        Vec3 reflected = reflect(r.d, normal);
        const auto pixel = trace(Ray(p + (cfg.bias * reflected), reflected, r.material), depth + 1);
        return Pixel{.color = pixel.color, .albedo = pixel.color, .normal = normal, .depth = t};
        break;
      }
    case Shading::REFRACTIVE:
      {
        Float cior = (r.material != 0) ? scene.materials(r.material).ior : 1.0f;
        Float nior = (r.material == obj) ? 1.0f : mat.ior;
        Vec3 refracted = refract(r.d, normal, cior / nior);
        const auto pixel = trace(Ray(p + (cfg.bias * refracted), refracted,
                                     (r.material == 0 || r.material != obj) ? obj : 0),
                                 depth + 1);
        return Pixel{.color = pixel.color, .albedo = Vec3(1.0f), .normal = normal, .depth = t};
        break;
      }
    case Shading::FRESNEL:
      {
        Float cior = (r.material != 0) ? scene.materials(r.material).ior : 1.0f;
        Float nior = (r.material == obj) ? 1.0f : mat.ior;
        Float r0 = (cior - nior) / (cior + nior);
        r0 *= r0;
        Float cos_theta = dot(-r.d, normal);
        Float fresnel = 0.0;
        if (cior > nior) {
          Float n = cior / nior;
          Float sin_theta = n * n * (1.0f - cos_theta * cos_theta);
          if (sin_theta > 1.0f) {
            fresnel = 1.0;
          } else {
            cos_theta = sqrt(1.0f - sin_theta);
            fresnel = r0 + (1.0f - r0) * pow(1.0f - cos_theta, 5);
          }
        } else {
          fresnel = r0 + (1.0f - r0) * pow(1.0f - cos_theta, 5);
        }

        if (frand() < fresnel) {
          Vec3 reflected = normalize(reflect(r.d, normal));
          const auto pixel =
              trace(Ray(p + (cfg.bias * reflected), reflected, r.material), depth + 1);
          return Pixel{.color = pixel.color, .albedo = pixel.color, .normal = normal, .depth = t};
        } else {
          Vec3 refracted = normalize(refract(r.d, normal, cior / nior));
          const auto pixel = trace(Ray(p + (cfg.bias * refracted), refracted,
                                       (r.material == 0 || r.material != obj) ? obj : 0),
                                   depth + 1);
          return Pixel{.color = pixel.color, .albedo = Vec3(1.0f), .normal = normal, .depth = t};
        }
        break;
      }
  }

  return Pixel();
}

std::tuple<trm::Float, std::uint32_t> trm::Renderer::ray_march(const Ray &r) const {
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
