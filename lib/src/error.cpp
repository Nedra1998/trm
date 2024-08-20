#include "error.hpp"

#include "log.hpp"

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

  const Float size = static_cast<Float>(reference.data.size());
  error.error /= size;
  error.absolute /= size;
  error.squared /= size;
  error.relative_absolute /= size;
  error.relative_squared /= size;

  return error;
}
