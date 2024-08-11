#include "img.hpp"

#include <filesystem>

#include <fmt/std.h>

#include "log.hpp"
#include "stb_image_write.h"

template <> struct fmt::formatter<trm::Buffer> : fmt::formatter<string_view> {
  auto format(trm::Buffer buf, format_context &ctx) const -> format_context::iterator {
    switch (buf) {
      case trm::Buffer::COLOR:
        return fmt::format_to(ctx.out(), "color");
      case trm::Buffer::ALBEDO:
        return fmt::format_to(ctx.out(), "albedo");
      case trm::Buffer::NORMAL:
        return fmt::format_to(ctx.out(), "normal");
      case trm::Buffer::DEPTH:
        return fmt::format_to(ctx.out(), "depth");
      case trm::Buffer::ALL:
        return fmt::format_to(ctx.out(), "all");
      default:
        return fmt::format_to(ctx.out(), "unknown");
    }
  }
};

bool trm::Image::write(std::string_view path, Buffer buffer) const {
  ZoneScoped;

  std::string_view ext = path.substr(path.find_last_of('.') + 1);
  if (ext == "png" || ext == "bmp" || ext == "tga" || ext == "jpg") {
    bool result = true;
    if (buffer & Buffer::COLOR)
      result |= write_u8(path, ext, Buffer::COLOR);
    if (buffer & Buffer::ALBEDO)
      result |= write_u8(path, ext, Buffer::ALBEDO);
    if (buffer & Buffer::NORMAL)
      result |= write_u8(path, ext, Buffer::NORMAL);
    if (buffer & Buffer::DEPTH)
      result |= write_u8(path, ext, Buffer::DEPTH);
    return result;
  } else if (ext == "hdr") {
    bool result = true;
    if (buffer & Buffer::COLOR)
      result |= write_f32(path, ext, Buffer::COLOR);
    if (buffer & Buffer::ALBEDO)
      result |= write_f32(path, ext, Buffer::ALBEDO);
    if (buffer & Buffer::NORMAL)
      result |= write_f32(path, ext, Buffer::NORMAL);
    if (buffer & Buffer::DEPTH)
      result |= write_f32(path, ext, Buffer::DEPTH);
    return result;
  } else {
    LOG_ERROR("Unsupported image extension \"{}\", must be one of [\"png\", \"bmp\", \"tga\", "
              "\"jpg\", \"hdr\"]",
              ext);
    return false;
  }
}

bool trm::Image::write_u8(std::string_view path, std::string_view ext, Buffer buffer) const {
  ZoneScoped;

  std::filesystem::path file(fmt::format(fmt::runtime(path), fmt::arg("buffer", buffer),
                                         fmt::arg("samples", samples), fmt::arg("width", width),
                                         fmt::arg("height", height)));

  LOG_INFO("Writing {} buffer to {} file {}", buffer, ext, file);

  std::size_t buffer_size = width * height * ((buffer == Buffer::DEPTH) ? 1 : 3);
  std::vector<uint8_t> buf(buffer_size);

  switch (buffer) {
    case Buffer::COLOR:
      {
        for (std::size_t i = 0; i < color.size(); ++i) {
          buf[i] = static_cast<uint8_t>(max(0.0f, min(1.0f, color[i])) * 255.0f);
        }
        break;
      }

    case Buffer::ALBEDO:
      {
        for (std::size_t i = 0; i < albedo.size(); ++i) {
          buf[i] = static_cast<uint8_t>(max(0.0f, min(1.0f, albedo[i])) * 255.0f);
        }
        break;
      }

    case Buffer::NORMAL:
      {
        for (std::size_t i = 0; i < normal.size(); i += 3) {
          if (normal[i] == 0.0f && normal[i + 1] == 0.0f && normal[i + 2] == 0.0f) {
            buf[i] = 0;
            buf[i + 1] = 0;
            buf[i + 2] = 0;
          } else {
            buf[i] =
                static_cast<uint8_t>((max(-1.0f, min(1.0f, normal[i])) * 0.5f + 0.5f) * 255.0f);
            buf[i + 1] =
                static_cast<uint8_t>((max(-1.0f, min(1.0f, normal[i + 1])) * 0.5f + 0.5f) * 255.0f);
            buf[i + 2] =
                static_cast<uint8_t>((max(-1.0f, min(1.0f, normal[i + 2])) * 0.5f + 0.5f) * 255.0f);
          }
        }
        break;
      }
    case Buffer::DEPTH:
      {
        Float min_depth = std::numeric_limits<Float>::infinity(), max_depth = 0.0f;
        for (std::size_t i = 0; i < depth.size(); ++i) {
          if (depth[i] < 0.0f)
            continue;
          if (depth[i] < min_depth)
            min_depth = depth[i];
          if (depth[i] > max_depth)
            max_depth = depth[i];
        }
        for (std::size_t i = 0; i < depth.size(); ++i) {
          if (depth[i] < 0.0f) {
            buf[i] = 0;
          } else {
            buf[i] = static_cast<uint8_t>(
                (1.0f - ((depth[i] - min_depth) / (max_depth - min_depth))) * 255.0f);
          }
        }
        break;
      }
    default:
      LOG_ERROR("Unsupported buffer type: {}", buffer);
      return false;
  }

  if (file.has_parent_path() && !std::filesystem::is_directory(file.parent_path())) {
    LOG_DEBUG("Creating output directory {}", file.parent_path());
    std::filesystem::create_directories(file.parent_path());
  }

  stbi_flip_vertically_on_write(true);
  if (ext == "png") {
    ZoneScopedN("stbi_write_png");
    return stbi_write_png(file.string().c_str(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                          buf.data(), width * ((buffer == Buffer::DEPTH) ? 1 : 3)) != 0;
  } else if (ext == "bmp") {
    ZoneScopedN("stbi_write_bmp");
    return stbi_write_bmp(file.string().c_str(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                          buf.data()) != 0;
  } else if (ext == "tga") {
    ZoneScopedN("stbi_write_tga");
    return stbi_write_tga(file.string().c_str(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                          buf.data()) != 0;
  } else if (ext == "jpg") {
    ZoneScopedN("stbi_write_jpg");
    return stbi_write_jpg(file.string().c_str(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                          buf.data(), 100) != 0;
  } else {
    LOG_ERROR(
        "Unsupported image format \"{}\", must be one of [\"png\", \"bmp\", \"tga\", \"jpg\"]",
        ext);
    return false;
  }
}

bool trm::Image::write_f32(std::string_view path, std::string_view ext, Buffer buffer) const {
  ZoneScoped;

  LOG_INFO("Writing {} buffer to {} file {}", buffer, ext, path);

  std::size_t buffer_size = width * height * ((buffer == Buffer::DEPTH) ? 1 : 3);
  std::vector<float> buf(buffer_size);

  switch (buffer) {
    case Buffer::COLOR:
      {
        for (std::size_t i = 0; i < color.size(); ++i) {
          buf[i] = static_cast<float>(max(0.0f, min(1.0f, color[i])));
        }
        break;
      }

    case Buffer::ALBEDO:
      {
        for (std::size_t i = 0; i < albedo.size(); ++i) {
          buf[i] = static_cast<float>(max(0.0f, min(1.0f, albedo[i])));
        }
        break;
      }

    case Buffer::NORMAL:
      {
        for (std::size_t i = 0; i < normal.size(); i += 3) {
          if (normal[i] == 0.0f && normal[i + 1] == 0.0f && normal[i + 2] == 0.0f) {
            buf[i] = 0;
            buf[i + 1] = 0;
            buf[i + 2] = 0;
          } else {
            buf[i] = static_cast<float>((max(-1.0f, min(1.0f, normal[i])) * 0.5f + 0.5f));
            buf[i + 1] = static_cast<float>((max(-1.0f, min(1.0f, normal[i + 1])) * 0.5f + 0.5f));
            buf[i + 2] = static_cast<float>((max(-1.0f, min(1.0f, normal[i + 2])) * 0.5f + 0.5f));
          }
        }
        break;
      }
    case Buffer::DEPTH:
      {
        Float min_depth = std::numeric_limits<Float>::infinity(), max_depth = 0.0f;
        for (std::size_t i = 0; i < depth.size(); ++i) {
          if (depth[i] < 0.0f)
            continue;
          if (depth[i] < min_depth)
            min_depth = depth[i];
          if (depth[i] > max_depth)
            max_depth = depth[i];
        }
        for (std::size_t i = 0; i < depth.size(); ++i) {
          if (depth[i] < 0.0f) {
            buf[i] = 0;
          } else {
            buf[i] =
                static_cast<float>((1.0f - ((depth[i] - min_depth) / (max_depth - min_depth))));
          }
        }
        break;
      }
    default:
      LOG_ERROR("Unsupported buffer type: {}", buffer);
      return false;
  }

  std::filesystem::path file(fmt::format(fmt::runtime(path), fmt::arg("buffer", buffer),
                                         fmt::arg("samples", samples), fmt::arg("width", width),
                                         fmt::arg("height", height)));

  if (file.has_parent_path() && !std::filesystem::is_directory(file.parent_path())) {
    LOG_DEBUG("Creating output directory {}", file.parent_path());
    std::filesystem::create_directories(file.parent_path());
  }

  stbi_flip_vertically_on_write(true);
  if (ext == "hdr") {
    ZoneScopedN("stbi_write_hdr");
    return stbi_write_hdr(file.string().c_str(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                          buf.data()) != 0;
  } else {
    LOG_ERROR("Unsupported image format \"{}\", must be one of [\"hdr\"]", ext);
    return false;
  }
}
