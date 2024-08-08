#include "ppm.hpp"

#include <cstring>
#include <format>
#include <fstream>

#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>

bool trm::ppm::write(std::string_view path, std::size_t width, std::size_t height, float *data,
                     bool binary) {
  ZoneScoped;
  SPDLOG_DEBUG("Writing PPM image to {}", path);
  std::ofstream file(path.data(), std::ios::binary);
  if (!file.is_open()) {
    SPDLOG_ERROR("Failed to open file {}: {}", path, std::strerror(errno));
    return false;
  }

  if (binary) {
    std::println(file, "P6");
  } else {
    std::println(file, "P3");
  }
  std::println(file, "{} {} {}", width, height, 255);

  if (binary) {
    std::vector<std::uint8_t> buffer(width * height * 3);
    for (std::size_t i = 0; i < width * height * 3; ++i) {
      buffer[i] = static_cast<std::uint8_t>(data[i] * 255);
    }
    file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
  } else {
    for (std::size_t row = 0; row < height; ++row) {
      for (std::size_t col = 0; col < width; ++col) {
        std::size_t index = row * width + col;
        std::uint8_t r = static_cast<std::uint8_t>(data[index * 3 + 0] * 255);
        std::uint8_t g = static_cast<std::uint8_t>(data[index * 3 + 1] * 255);
        std::uint8_t b = static_cast<std::uint8_t>(data[index * 3 + 2] * 255);
        std::print(file, "{:3d} {:3d} {:3d} ", r, g, b);
      }
      std::println(file);
    }
  }

  file.close();
  return true;
}
