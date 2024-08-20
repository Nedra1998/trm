/**
 * @file error.hpp
 * @brief Error calculatio for computing the difference between images
 *
 * This file provides an interface for computing the error between images. There are a number of
 * different metrics that are supported.
 *
 * @todo Add support for \f$\Delta E\f$ error metric, which provides a perceptual difference between
 * images.
 */

#ifndef INCLUDE_TRM_ERROR_HPP_
#define INCLUDE_TRM_ERROR_HPP_

#include <cstdint>

#include "trm/image.hpp"

namespace trm {
  /**
   * @brief The error metric to use when computing the difference between images
   *
   * Given a reference image \f$r\f$ and a test image \f$i\f$ the following metrics are supported:
   */
  enum struct ErrorMetric : std::uint8_t {
    // clang-format off
    ERROR             = 0b00001, ///< \f$i - r\f$
    ABSOLUTE          = 0b00010, ///< \f$|i - r|\f$
    SQUARED           = 0b00100, ///< \f$(i - r)^2\f$
    RELATIVE_ABSOLUTE = 0b01000, ///< \f$\frac{|i - r|}{r + 0.01}\f$
    RELATIVE_SQUARED  = 0b10000, ///< \f$\frac{(i - r)^2}{r^2 + 0.01}\f$
    ALL               = 0b11111, ///< All metrics
    // clang-format on
  };

  // clang-format off
  inline ErrorMetric operator~(ErrorMetric a) { return (ErrorMetric) ~(std::uint32_t)a; }
  inline ErrorMetric operator|(ErrorMetric a, ErrorMetric b) { return (ErrorMetric)((std::uint32_t)a | (std::uint32_t)b); }
  inline ErrorMetric operator&(ErrorMetric a, ErrorMetric b) { return (ErrorMetric)((std::uint32_t)a & (std::uint32_t)b); }
  inline ErrorMetric operator^(ErrorMetric a, ErrorMetric b) { return (ErrorMetric)((std::uint32_t)a ^ (std::uint32_t)b); }
  inline ErrorMetric &operator|=(ErrorMetric &a, ErrorMetric b) { return (ErrorMetric &)((std::uint32_t &)a |= (std::uint32_t)b); }
  inline ErrorMetric &operator&=(ErrorMetric &a, ErrorMetric b) { return (ErrorMetric &)((std::uint32_t &)a &= (std::uint32_t)b); }
  inline ErrorMetric &operator^=(ErrorMetric &a, ErrorMetric b) { return (ErrorMetric &)((std::uint32_t &)a ^= (std::uint32_t)b); }
  // clang-format on

  /**
   * @class Error
   * @brief The error between two images
   *
   * This struct contains the error between two images. Only the `ErrorMetric` configured when
   * computing the error will have their values set, the others will be `0.0f`.
   */
  struct Error {
    float error = 0.0f, absolute = 0.0f, squared = 0.0f, relative_absolute = 0.0f,
          relative_squared = 0.0f;
  };

  /**
   * @brief Compute the difference between two images
   *
   * This function computes the difference between two images using the configured error metric. It
   * computes the sum of the error metric for each pixel in the image, then normalize the result by
   * the number of pixels in the image.
   *
   * @param reference The reference image.
   * @param b The test image.
   * @param metric The error metric(s) to compute.
   * @return The error between the two images.
   */
  Error compare(const Image &reference, const Image &test, ErrorMetric metric = ErrorMetric::ALL);

  /**
   * @brief Generate an image showing the difference between two images.
   *
   * This function generates an image representing the difference between the two provided images.
   * It applies the configured error metric to each pixel in the image and uses the resulting value
   * as the pixel value in the output image.
   *
   * @param reference The reference image.
   * @param test The test image.
   * @param metric THe error metric to use when computing the difference.
   * @return An image showing the difference between the two images using the given metric.
   */
  Image difference(const Image &reference, const Image &test,
                   ErrorMetric metric = ErrorMetric::ABSOLUTE);

  /**
   * @brief Compute the difference between two images
   *
   * This is a convenience function that computes the difference between two images using the
   * `ErrorMetric::ABSOLUTE`.
   *
   * @param a The reference images.
   * @param b The test image.
   * @return An image representing the difference between the two images.
   * @see difference
   */
  inline Image operator-(const Image &a, const Image &b) { return difference(a, b); }
} // namespace trm

#endif // INCLUDE_TRM_ERROR_HPP_
