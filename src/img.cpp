#include "img.hpp"

#include <filesystem>

#include <fmt/std.h>

#include "log.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include "version.hpp"

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

std::string trm::Image::path(std::string_view path, Buffer buffer) const {
  return fmt::format(fmt::runtime(path), fmt::arg("buffer", buffer), fmt::arg("samples", samples),
                     fmt::arg("width", width), fmt::arg("height", height),
                     fmt::arg("commit", TRM_VERSION_COMMIT), fmt::arg("name", name));
}

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

bool trm::Image::read(std::string_view path, Buffer buffer) {
  ZoneScoped;

  LOG_INFO("Reading {} buffer from file {}", buffer, path);

  int x, y, n;
  float *data = stbi_loadf(path.data(), &x, &y, &n, (buffer == Buffer::DEPTH) ? 1 : 3);
  if (data == nullptr) {
    LOG_ERROR("Failed to read image {}", path);
    return false;
  }

  width = x;
  height = y;
  samples = 0;
  switch (buffer) {
    case Buffer::COLOR:
      color.resize(width * height * 3);
      std::copy(data, data + color.size(), color.begin());
      break;
    case Buffer::ALBEDO:
      albedo.resize(width * height * 3);
      std::copy(data, data + albedo.size(), albedo.begin());
      break;
    case Buffer::NORMAL:
      normal.resize(width * height * 3);
      std::copy(data, data + normal.size(), normal.begin());
      break;
    case Buffer::DEPTH:
      depth.resize(width * height);
      std::copy(data, data + depth.size(), depth.begin());
      break;
    default:
      LOG_ERROR("Unsupported buffer type: {}", buffer);
      return false;
  }

  stbi_image_free(data);
  return true;
}

trm::ImageError trm::Image::error(const Image &other) const {
  ZoneScoped;
  LOG_INFO("Computing error between images");
  if (width != other.width || height != other.height) {
    LOG_ERROR("Image dimensions do not match: {}x{} != {}x{}", width, height, other.width,
              other.height);
    return ImageError{.error = -1.0};
  }

  ImageError error;
  for (std::size_t i = 0; i < color.size(); ++i) {
    float diff = other.color[i] - color[i];
    error.error += diff;
    error.absolute += std::abs(diff);
    error.squared += diff * diff;
    error.relative_absolute += std::abs(diff) / (color[i] + 0.001);
    error.relative_squared += diff * diff / (color[i] * color[i] + 0.001);
  }
  error.error /= color.size();
  error.absolute /= color.size();
  error.squared /= color.size();
  error.relative_absolute /= color.size();
  error.relative_squared /= color.size();

  LOG_INFO("Error: error={:.6f} absolute={:.6f} squared={:.6f} relative_absolute={:.6f} "
           "relative_squared={:.6f}",
           error.error, error.absolute, error.squared, error.relative_absolute,
           error.relative_squared);
  return error;
}

bool trm::Image::normalize(Buffer buffer) {
  ZoneScoped;

  LOG_DEBUG("Normalizing image {} buffer", buffer);
  if (buffer & Buffer::COLOR) {
    for (std::size_t i = 0; i < color.size(); ++i) {
      color[i] = max(0.0f, min(1.0f, color[i]));
    }
  }

  if (buffer & Buffer::ALBEDO) {
    for (std::size_t i = 0; i < albedo.size(); ++i) {
      albedo[i] = max(0.0f, min(1.0f, albedo[i]));
    }
  }

  if (buffer & Buffer::NORMAL) {
    for (std::size_t i = 0; i < normal.size(); i += 3) {
      if (normal[i] == 0.0f && normal[i + 1] == 0.0f && normal[i + 2] == 0.0f) {
        normal[i] = 0;
        normal[i + 1] = 0;
        normal[i + 2] = 0;
      } else {
        normal[i] = (max(-1.0f, min(1.0f, normal[i])) * 0.5f + 0.5f);
        normal[i + 1] = (max(-1.0f, min(1.0f, normal[i + 1])) * 0.5f + 0.5f);
        normal[i + 2] = (max(-1.0f, min(1.0f, normal[i + 2])) * 0.5f + 0.5f);
      }
    }
  }
  if (buffer & Buffer::DEPTH) {
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
        depth[i] = 0;
      } else {
        depth[i] = (1.0f - ((depth[i] - min_depth) / (max_depth - min_depth)));
      }
    }
  }
  return true;
}

bool trm::Image::write_u8(std::string_view path, std::string_view ext, Buffer buffer) const {
  ZoneScoped;

  std::filesystem::path file(this->path(path, buffer));

  LOG_INFO("Writing {} buffer to {} file {}", buffer, ext, file);

  if (file.has_parent_path() && !std::filesystem::is_directory(file.parent_path())) {
    LOG_DEBUG("Creating output directory {}", file.parent_path());
    std::filesystem::create_directories(file.parent_path());
  }

  std::size_t buffer_size = width * height * ((buffer == Buffer::DEPTH) ? 1 : 3);
  std::vector<uint8_t> buf(buffer_size);

  switch (buffer) {
    case Buffer::COLOR:
      std::transform(color.cbegin(), color.cend(), buf.begin(),
                     [](Float v) { return static_cast<uint8_t>(255.0f * v); });
      break;

    case Buffer::ALBEDO:
      std::transform(albedo.cbegin(), albedo.cend(), buf.begin(),
                     [](Float v) { return static_cast<uint8_t>(255.0f * v); });
      break;

    case Buffer::NORMAL:
      std::transform(normal.cbegin(), normal.cend(), buf.begin(),
                     [](Float v) { return static_cast<uint8_t>(255.0f * v); });
      break;
    case Buffer::DEPTH:
      std::transform(depth.cbegin(), depth.cend(), buf.begin(),
                     [](Float v) { return static_cast<uint8_t>(255.0f * v); });
      break;
    default:
      LOG_ERROR("Unsupported buffer type: {}", buffer);
      return false;
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

  std::filesystem::path file(this->path(path, buffer));

  LOG_INFO("Writing {} buffer to {} file {}", buffer, ext, file);

  if (file.has_parent_path() && !std::filesystem::is_directory(file.parent_path())) {
    LOG_DEBUG("Creating output directory {}", file.parent_path());
    std::filesystem::create_directories(file.parent_path());
  }

  std::size_t buffer_size = width * height * ((buffer == Buffer::DEPTH) ? 1 : 3);
  std::vector<float> buf(buffer_size);

  switch (buffer) {
    case Buffer::COLOR:
      std::copy(color.begin(), color.end(), buf.begin());
      break;

    case Buffer::ALBEDO:
      std::copy(albedo.begin(), albedo.end(), buf.begin());
      break;

    case Buffer::NORMAL:
      std::copy(normal.begin(), normal.end(), buf.begin());
      break;
    case Buffer::DEPTH:
      std::copy(depth.begin(), depth.end(), buf.begin());
      break;
    default:
      LOG_ERROR("Unsupported buffer type: {}", buffer);
      return false;
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
