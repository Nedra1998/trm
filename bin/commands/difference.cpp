#include <filesystem>

#include <fmt/std.h>
#include <trm/error.hpp>
#include <trm/image.hpp>
#include <trm/log.hpp>

#include "commands.hpp"

int difference(argparse::ArgumentParser &parser) {
  std::filesystem::path refpath = parser.get<std::string>("reference");
  std::filesystem::path testpath = parser.get<std::string>("test");

  std::error_code ec;
  if (!std::filesystem::exists(refpath, ec)) {
    LOG_ERROR("Reference image path '{}' does not exist: {}", refpath, ec.message());
    return 1;
  }
  if (!std::filesystem::exists(testpath, ec)) {
    LOG_ERROR("Test image path '{}' does not exist: {}", testpath, ec.message());
    return 1;
  }

  const std::optional<trm::Image> refopt = trm::Image::read(refpath.string());
  if (!refopt) {
    LOG_ERROR("Failed to read reference image '{}'", refpath);
    return 1;
  }

  const std::optional<trm::Image> testopt = trm::Image::read(testpath.string());
  if (!testopt) {
    LOG_ERROR("Failed to read test image '{}'", testpath);
    return 1;
  }

  const trm::Image &ref = *refopt;
  const trm::Image &test = *testopt;

  const trm::Error error = trm::compare(ref, test);
  const trm::Image diff = trm::difference(ref, test);

  diff.write(parser.get<std::string>("--output"));

  fmt::println("REF={}", refpath);
  fmt::println("TEST={}", testpath);
  fmt::println("DIFF={}", parser.get<std::string>("--output"));
  fmt::println("  Error: {}", error.error);
  fmt::println("  Absolute Error: {}", error.absolute);
  fmt::println("  Squared Error: {}", error.squared);
  fmt::println("  Relative Absolute Error: {}", error.relative_absolute);
  fmt::println("  Relative Squared Error: {}", error.relative_squared);
  return 0;
}
