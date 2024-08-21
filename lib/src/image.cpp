#include "image.hpp"

#include <filesystem>

#include "log.hpp"
#include "version.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "md5.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

std::optional<trm::Image> trm::Image::read(std::string_view path) {
  PROFILE_ZONE();

  std::filesystem::path file = path;
  if (!std::filesystem::exists(file)) {
    LOG_ERROR("File does not exist: {}", path);
    return std::nullopt;
  }

  int width, height, channels;
  float *data = nullptr;
  {
    PROFILE_ZONEN("stbi_loadf");
    data = stbi_loadf(path.data(), &width, &height, &channels, 0);
  }
  if (data == nullptr) {
    LOG_ERROR("Failed to read image: {}", path);
    return std::nullopt;
  }

  Image image(width, height, channels);
  std::copy(data, data + width * height * channels, image.data.begin());

  stbi_image_free(data);
  return image;
}

bool trm::Image::write(std::string_view path) const {
  PROFILE_ZONE();

  std::filesystem::path file = path;
  if (file.has_parent_path() && !std::filesystem::is_directory(file.parent_path())) {
    LOG_DEBUG("Creating directory for file: {}", file.parent_path().string());
    std::error_code err;
    if (!std::filesystem::create_directories(file.parent_path(), err)) {
      LOG_ERROR("Failed to create directory for file: {}", err.message());
      return false;
    }
  }

  std::string_view extension = path.substr(path.find_last_of('.') + 1);
  std::vector<Float> data = normalized();

  stbi_flip_vertically_on_write(true);
  if (extension == "hdr") {
    PROFILE_ZONEN("stbi_write_hdr");
    return stbi_write_hdr(file.c_str(), width, height, depth, data.data()) != 0;
  } else {
    std::vector<std::uint8_t> pixels(data.size());
    std::transform(data.begin(), data.end(), pixels.begin(),
                   [](Float value) { return static_cast<std::uint8_t>(value * 255.0f); });

    if (extension == "png") {
      PROFILE_ZONEN("stbi_write_png");
      return stbi_write_png(file.c_str(), width, height, depth, pixels.data(), width * depth) != 0;
    } else if (extension == "jpg" || extension == "jpeg") {
      PROFILE_ZONEN("stbi_write_jpg");
      return stbi_write_jpg(file.c_str(), width, height, depth, pixels.data(), 100) != 0;
    } else if (extension == "bmp") {
      PROFILE_ZONEN("stbi_write_bmp");
      return stbi_write_bmp(file.c_str(), width, height, depth, pixels.data()) != 0;
    } else if (extension == "tga") {
      PROFILE_ZONEN("stbi_write_tga");
      return stbi_write_tga(file.c_str(), width, height, depth, pixels.data()) != 0;
    } else {
      LOG_ERROR(
          "Unsupported image format: {}, supported formats are: hdr, png, jpg, jpeg, bmp, tga",
          extension);
      return false;
    }
  }

  return false;
}

std::vector<trm::Float> trm::Image::normalized() const {
  PROFILE_ZONE();

  std::vector<Float> normalized_data(data.size());

  switch (buffer) {
    case Buffer::COLOR:
      std::transform(data.cbegin(), data.cend(), normalized_data.begin(),
                     [](Float value) { return std::clamp(value, 0.0f, 1.0f); });
      break;
    case Buffer::ALBEDO:
      std::transform(data.cbegin(), data.cend(), normalized_data.begin(),
                     [](Float value) { return std::clamp(value, 0.0f, 1.0f); });
      break;
    case Buffer::NORMAL:
      std::transform(data.cbegin(), data.cend(), normalized_data.begin(),
                     [](Float value) { return (std::clamp(value, -1.0f, 1.0f) + 1.0f) / 2.0f; });
      break;
    case Buffer::DEPTH:
      {
        Float min = *std::min_element(data.cbegin(), data.cend());
        Float max = *std::max_element(data.cbegin(), data.cend());
        if (max >= 100.0f)
          max *= 0.95f;
        std::transform(data.cbegin(), data.cend(), normalized_data.begin(),
                       [&](Float value) { return 1.0f - ((value - min) / (max - min)); });
        break;
      }
    default:
      std::copy(data.cbegin(), data.cend(), normalized_data.begin());
      break;
  }

  return normalized_data;
}

std::string trm::Image::md5() const {
  PROFILE_ZONE();

  MD5 md5;
  md5(data.data(), data.size() * sizeof(Float));
  return md5.getHash();
}

trm::ImageSet::ImageSet(std::uint32_t width, std::uint32_t height, Buffer buffers)
    : width(width), height(height), samples(0),
      color(((buffers & Buffer::COLOR) == Buffer::COLOR) ? width : 0,
            ((buffers & Buffer::COLOR) == Buffer::COLOR) ? height : 0, Buffer::COLOR),
      albedo(((buffers & Buffer::ALBEDO) == Buffer::ALBEDO) ? width : 0,
             ((buffers & Buffer::ALBEDO) == Buffer::ALBEDO) ? height : 0, Buffer::ALBEDO),
      normal(((buffers & Buffer::NORMAL) == Buffer::NORMAL) ? width : 0,
             ((buffers & Buffer::NORMAL) == Buffer::NORMAL) ? height : 0, Buffer::NORMAL),
      depth(((buffers & Buffer::DEPTH) == Buffer::DEPTH) ? width : 0,
            ((buffers & Buffer::DEPTH) == Buffer::DEPTH) ? height : 0, Buffer::DEPTH) {}

std::string trm::ImageSet::fmt(std::string_view pattern, Buffer buffer) const {
  return fmt::format(fmt::runtime(pattern), fmt::arg("buffer", buffer),
                     fmt::arg("samples", samples), fmt::arg("width", width),
                     fmt::arg("height", height), fmt::arg("commit", TRM_VERSION_COMMIT));
}

bool trm::ImageSet::write(std::string_view pattern, Buffer buffer) const {
  PROFILE_ZONE();

  bool result = true;
  if ((buffer & Buffer::COLOR) == Buffer::COLOR && color.width != 0) {
    result &= color.write(fmt(pattern, color.buffer));
  }
  if ((buffer & Buffer::ALBEDO) == Buffer::ALBEDO && albedo.width != 0) {
    result &= albedo.write(fmt(pattern, albedo.buffer));
  }
  if ((buffer & Buffer::NORMAL) == Buffer::NORMAL && normal.width != 0) {
    result &= normal.write(fmt(pattern, normal.buffer));
  }
  if ((buffer & Buffer::DEPTH) == Buffer::DEPTH && depth.width != 0) {
    result &= depth.write(fmt(pattern, depth.buffer));
  }

  return result;
}

auto fmt::formatter<trm::Buffer>::format(trm::Buffer buf,
                                         format_context &ctx) const -> format_context::iterator {
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
