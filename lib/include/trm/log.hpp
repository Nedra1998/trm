/**
 * @file log.hpp
 * @brief TRM logging utilities
 *
 * This file contains logging utilities for TRM. It uses the `spdlog` library
 * for logging and `fmt` for formatting. It also includes `Tracy` for profiling
 * support.
 */

#ifndef TRM_LOG_HPP_
#define TRM_LOG_HPP_

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>

/// @brief Log a trace message
#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
/// @brief Log a debug message
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
/// @brief Log an info message
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
/// @brief Log a warning message
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
/// @brief Log an error message
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
/// @brief Log a critical error message
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

#define LOG_ASSERT(...)                                                                            \
  if (!(__VA_ARGS__)) {                                                                            \
    LOG_CRITICAL("Assertion failed: {}", #__VA_ARGS__);                                            \
    std::abort();                                                                                  \
  }

/// @brief Enable profiling for the current scope
#define PROFILE_ZONE() ZoneScoped

/// @brief Enable profiling for the current scope with a custom name
#define PROFILE_ZONEN(name) ZoneScopedN(name)

#endif // TRM_LOG_HPP_
