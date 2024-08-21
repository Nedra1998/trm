/**
 * @file trm.hpp
 * @brief The primary TRM header file.
 *
 * This file provides the primary entrypoint for TRM and should be used by the library users. It
 * provides an interface to all the rendering features, and includes all the required headers.
 */

#ifndef TRM_HPP_
#define TRM_HPP_

#include "trm/config.hpp"
#include "trm/error.hpp"
#include "trm/float.hpp"
#include "trm/log.hpp"
#include "trm/material.hpp"
#include "trm/renderer.hpp"
#include "trm/scene.hpp"
#include "trm/version.hpp"

/**
 * @brief The primary TRM namespace
 *
 * This namespace contains all TRM code. It is used to prevent name collisions
 * with other libraries.
 */
namespace trm {}

#endif // TRM_HPP_
