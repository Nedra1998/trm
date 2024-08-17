/**
 * @file rand.hpp
 * @brief TRM random number utilities
 *
 * This file contains utilities for generating random numbers in TRM. It uses the C++11 random
 * number generation facilities.
 */

#ifndef TRM_RAND_HPP_
#define TRM_RAND_HPP_

#include <random>

#include "trm/float.hpp"

namespace trm {
  /// @brief Random device used to seed the random number generator
  static std::random_device rd;
  /// @brief Mersenne Twister random number generator
  static std::mt19937 gen(rd());
  /// @brief Uniform integer distribution with range \f$[0, \text{INT_MAX}]\f$.
  static std::uniform_int_distribution<int> idist(0, std::numeric_limits<int>::max());
  /// @brief Uniform float distribution with range \f$[0, 1)\f$.
  static std::uniform_real_distribution<Float> dist(0.0f, 1.0f);
  /// @brief Uniform float distribution with range \f$[-1, 1)\f$.
  static std::uniform_real_distribution<Float> dist2(-1.0f, 1.0f);
  /// @brief Generate a random integer in the range \f$[0, \text{INT_MAX}]\f$.
  inline int irand() { return idist(gen); }
  /// @brief Generate a random float in the range \f$[0, 1)\f$.
  inline Float frand() { return dist(gen); }
  /// @brief Generate a random float in the range \f$[-1, 1)\f$.
  inline Float frand2() { return dist2(gen); }
  /**
   * @brief Generate a random float in the range \f$[\text{min}, \text{max})\f$.
   *
   * @param min Minimum value of the range
   * @param max Maximum value
   * @return Random float in the range \f$[\text{min}, \text{max})\f$
   */
  inline Float frand(const Float &min, const Float &max) { return dist(gen) * (max - min) + min; }
} // namespace trm

#endif // TRM_RAND_HPP_
