#include "error.hpp"

#include "log.hpp"

using Vec3 = trm::Vec3;

float ciede76(trm::Vec3 ref, trm::Vec3 tst) {
  const glm::vec3 ref_xyz(0.4124564f * ref.r + 0.3575761f * ref.g + 0.1804375f * ref.b,
                          0.2126729f * ref.r + 0.7151522f * ref.g + 0.0721750f * ref.b,
                          0.0193339f * ref.r + 0.1191920f * ref.g + 0.9503041f * ref.b);
  const glm::vec3 tst_xyz(0.4124564f * tst.r + 0.3575761f * tst.g + 0.1804375f * tst.b,
                          0.2126729f * tst.r + 0.7151522f * tst.g + 0.0721750f * tst.b,
                          0.0193339f * tst.r + 0.1191920f * tst.g + 0.9503041f * tst.b);

  auto f = [](float t) {
    const float delta = 6.0f / 29.0f;
    if (t > glm::pow(delta, 3.0f)) {
      return glm::pow(t, 1.0f / 3.0f);
    } else {
      return t / (3.0f * glm::pow(delta, 2.0f)) + 4.0f / 29.0f;
    }
  };

  const glm::vec3 ref_lab(116.0f * f(ref_xyz.y) - 16.0f, 500.0f * (f(ref_xyz.x) - f(ref_xyz.y)),
                          200.0f * (f(ref_xyz.y) - f(ref_xyz.z)));
  const glm::vec3 tst_lab(116.0f * f(tst_xyz.y) - 16.0f, 500.0f * (f(tst_xyz.x) - f(tst_xyz.y)),
                          200.0f * (f(tst_xyz.y) - f(tst_xyz.z)));
  return glm::length(tst_lab - ref_lab);
}

trm::Error trm::compare(const Image &reference, const Image &test, ErrorMetric metric) {
  PROFILE_ZONE();

  if (reference.width != test.width || reference.height != test.height ||
      reference.depth != test.depth) {
    LOG_ERROR("Image dimensions do not match: reference({}, {}, {}), test({}, {}, {})",
              reference.width, reference.height, reference.depth, test.width, test.height,
              test.depth);
    return Error{.error = -1.0f};
  }

  Error error;
  for (std::size_t i = 0; i < reference.data.size(); ++i) {
    const Float r = reference.data[i];
    const Float t = test.data[i];
    const Float diff = t - r;

    error.error += diff;
    error.absolute += std::abs(diff);
    error.squared += diff * diff;
    error.relative_absolute += std::abs(diff) / (r + 0.01f);
    error.relative_squared += diff * diff / (r * r + 0.01f);
  }

  for (std::size_t i = 0; i < reference.data.size(); i += 3) {
    const Vec3 ref(reference.data[i], reference.data[i + 1], reference.data[i + 2]);
    const Vec3 tst(test.data[i], test.data[i + 1], test.data[i + 2]);
    error.delta_e += ciede76(ref, tst);
  }

  const Float size = static_cast<Float>(reference.data.size());
  error.error /= size;
  error.absolute /= size;
  error.squared /= size;
  error.relative_absolute /= size;
  error.relative_squared /= size;
  error.delta_e /= (size / 3.0);

  return error;
}

trm::Image trm::difference(const Image &reference, const Image &test, ErrorMetric metric) {
  PROFILE_ZONE();

  if (reference.width != test.width || reference.height != test.height ||
      reference.depth != test.depth) {
    LOG_ERROR("Image dimensions do not match: reference({}, {}, {}), test({}, {}, {})",
              reference.width, reference.height, reference.depth, test.width, test.height,
              test.depth);
    return Image(0, 0);
  }
  Image diff(reference.width, reference.height, reference.depth);
  for (std::size_t i = 0; i < reference.data.size(); ++i) {
    const Float r = reference.data[i];
    const Float t = test.data[i];
    switch (metric) {
      case ErrorMetric::ERROR:
        diff.data[i] = t - r;
        break;
      case ErrorMetric::ABSOLUTE:
        diff.data[i] = std::fabs(t - r);
        break;
      case ErrorMetric::SQUARED:
        diff.data[i] = (t - r) * (t - r);
        break;
      case ErrorMetric::RELATIVE_ABSOLUTE:
        diff.data[i] = std::abs(t - r) / (r + 0.01f);
        break;
      case ErrorMetric::RELATIVE_SQUARED:
        diff.data[i] = (t - r) * (t - r) / (r * r + 0.01f);
        break;
      default:
        LOG_ERROR("Unsupported error metric for image difference");
        break;
    }
  }
  return diff;
}
