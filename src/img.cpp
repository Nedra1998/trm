#include "img.hpp"

#include <spdlog/spdlog.h>

#include "stb_image_write.h"

template <> struct std::formatter<trm::Buffer, char> : std::formatter<std::string, char> {
  auto format(trm::Buffer v, std::format_context &ctx) {
    std::string value_name;
    switch (v) {
    case trm::Buffer::COLOR:
      value_name = "color";
      break;
    case trm::Buffer::DEPTH:
      value_name = "depth";
      break;
    case trm::Buffer::NORMAL:
      value_name = "normal";
      break;
    case trm::Buffer::ALBIDO:
      value_name = "albido";
      break;
    }
    return std::formatter<std::string>::format(value_name, ctx);
  }
};

bool trm::Image::write(std::string_view path, Buffer buffer) {

  std::size_t buffer_size = width * height * ((buffer == Buffer::DEPTH) ? 1 : 3);
  std::string_view ext = path.substr(path.find_last_of('.') + 1);
  if (ext == "png" || ext == "bmp" || ext == "tga" || ext == "jpg") {
    std::vector<uint8_t> img(buffer_size);

    switch (buffer) {
    case Buffer::COLOR:
      for (std::size_t i = 0; i < color.size(); ++i) {
        img[i] = static_cast<uint8_t>(color[i] * 255.0f);
      }
      break;
    case Buffer::ALBIDO:
      for (std::size_t i = 0; i < albido.size(); ++i) {
        img[i] = static_cast<uint8_t>(albido[i] * 255.0f);
      }
      break;
    case Buffer::NORMAL:
      for (std::size_t i = 0; i < normal.size(); i += 3) {
        if (normal[i] == 0.0f && normal[i + 1] == 0.0f && normal[i + 2] == 0.0f) {
          img[i] = 0;
          img[i + 1] = 0;
          img[i + 2] = 0;
          continue;
        } else {
          img[i] = static_cast<uint8_t>((normal[i] * 0.5f + 0.5f) * 255.0f);
          img[i + 1] = static_cast<uint8_t>((normal[i + 1] * 0.5f + 0.5f) * 255.0f);
          img[i + 2] = static_cast<uint8_t>((normal[i + 2] * 0.5f + 0.5f) * 255.0f);
        }
      }
      break;
    case Buffer::DEPTH:
      Float min_depth = std::numeric_limits<Float>::infinity(), max_depth = 0.0f;
      for (std::size_t i = 0; i < depth.size(); ++i) {
        if (depth[i] < min_depth)
          min_depth = depth[i];
        if (depth[i] >= 100.0f)
          continue;
        if (depth[i] > max_depth)
          max_depth = depth[i];
      }
      for (std::size_t i = 0; i < depth.size(); ++i) {
        if (depth[i] >= max_depth) {
          img[i] = 0;
        } else {
          img[i] = static_cast<uint8_t>(
              (1.0f - ((depth[i] - min_depth) / (max_depth - min_depth))) * 255.0f);
        }
      }
      break;
    }

    stbi_flip_vertically_on_write(true);
    if (ext == "png") {
      // SPDLOG_INFO("Writting {} buffer to png file {}", buffer, path);
      return stbi_write_png(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                            img.data(), width * ((buffer == Buffer::DEPTH) ? 1 : 3)) != 0;
    } else if (ext == "bmp") {
      // SPDLOG_INFO("Writting {} buffer to bmp file {}", buffer, path);
      return stbi_write_bmp(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                            img.data()) != 0;
    } else if (ext == "tga") {
      // SPDLOG_INFO("Writting {} buffer to tga file {}", buffer, path);
      return stbi_write_tga(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                            img.data()) != 0;
    } else if (ext == "jpg") {
      // SPDLOG_INFO("Writting {} buffer to jpg file {}", buffer, path);
      return stbi_write_jpg(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                            img.data(), 100) != 0;
    }
    return false;
  } else if (ext == "hdr") {
    std::vector<float> img(buffer_size);

    switch (buffer) {
    case Buffer::COLOR:
      for (std::size_t i = 0; i < color.size(); ++i) {
        img[i] = static_cast<float>(color[i]);
      }
      break;
    case Buffer::ALBIDO:
      for (std::size_t i = 0; i < albido.size(); ++i) {
        img[i] = static_cast<float>(albido[i]);
      }
      break;
    case Buffer::NORMAL:
      for (std::size_t i = 0; i < normal.size(); ++i) {
        img[i] = static_cast<float>(normal[i]);
      }
      break;
    case Buffer::DEPTH:
      for (std::size_t i = 0; i < depth.size(); ++i) {
        img[i] = static_cast<float>(depth[i]);
      }
      break;
    }
    // SPDLOG_INFO("Writting {} buffer to hdr file {}", buffer, path);
    return stbi_write_hdr(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                          img.data()) != 0;
  } else {
    SPDLOG_ERROR("Unsupported image format, only support png, bmp, tga, jpg, hdr");
    return false;
  }
}
