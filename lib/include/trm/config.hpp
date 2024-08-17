/**
 * @file config.hpp
 * @brief Configuration types for TRM
 *
 * This file contains the configuration types used to configure the TRM renderer.
 */

#ifndef TRM_CONFIG_HPP_
#define TRM_CONFIG_HPP_

#include <cstdint>

#include "trm/float.hpp"

namespace trm {

  /**
   * @brief Features that can be enabled in the TRM renderer
   *
   * These features can be enabled in the renderer to provide additional
   * functionality. They are used as a bitflag in the renderer configuration.
   */
  enum struct Feature : std::uint32_t {
    // clang-format off
    NONE            = 0,      ///< No features enabled
    PROGRESS_REPORT = 1 << 0, ///< Log a progress report at regular intervals during rendering.
    // clang-format on
  };

  // clang-format off
  inline Feature operator~(Feature a) { return (Feature) ~(std::uint32_t)a; }
  inline Feature operator|(Feature a, Feature b) { return (Feature)((std::uint32_t)a | (std::uint32_t)b); }
  inline Feature operator&(Feature a, Feature b) { return (Feature)((std::uint32_t)a & (std::uint32_t)b); }
  inline Feature operator^(Feature a, Feature b) { return (Feature)((std::uint32_t)a ^ (std::uint32_t)b); }
  inline Feature &operator|=(Feature &a, Feature b) { return (Feature &)((std::uint32_t &)a |= (std::uint32_t)b); }
  inline Feature &operator&=(Feature &a, Feature b) { return (Feature &)((std::uint32_t &)a &= (std::uint32_t)b); }
  inline Feature &operator^=(Feature &a, Feature b) { return (Feature &)((std::uint32_t &)a ^= (std::uint32_t)b); }
  // clang-format on

  /**
   * @brief Configuration for the TRM renderer
   *
   * This struct contains the configuration options for the TRM renderer. It is
   * used to configure how the renderer will operate. Each field is described in
   * more detail with how it will impact the redering process.
   */
  struct Config {
    // clang-format off
    std::uint32_t max_depth; ///< Maximum depth of the raytracing recursion.
    std::uint32_t max_steps; ///< Maximum number of steps in the ray marching algorithm.
    Float max_distance;      ///< Maximum distance before a ray is considered to have missed all objects.
    Float epsilon;           ///< Distance threshold to consider a ray-object intersection.
    Float bias;              ///< Bias to apply on ray-object intersections to prevent self-intersections.
    Feature features;       ///< Bitflag of features to enable in the renderer.
    // clang-format on
  };
} // namespace trm

#endif // TRM_CONFIG_HPP_
