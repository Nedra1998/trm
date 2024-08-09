#include "img.hpp"

#include <spdlog/spdlog.h>

#include "stb_image_write.h"

bool trm::Image::write(std::string_view path, Buffer buffer) {

  if (buffer & Buffer::COLOR && color.empty()) {
    SPDLOG_ERROR("Color buffer is empty");
    return false;
  } else if (buffer & Buffer::ALBIDO && albido.empty()) {
    SPDLOG_ERROR("Albido buffer is empty");
    return false;
  } else if (buffer & Buffer::NORMAL && normal.empty()) {
    SPDLOG_ERROR("Normal buffer is empty");
    return false;
  } else if (buffer & Buffer::DEPTH && depth.empty()) {
    SPDLOG_ERROR("Depth buffer is empty");
    return false;
  }

  std::string_view buffer_str;
  std::size_t buffer_size = width * height * ((buffer == Buffer::DEPTH) ? 1 : 3);
  std::vector<Float>::const_iterator it, end;
  switch (buffer) {
  case Buffer::COLOR:
    buffer_str = "color";
    it = color.cbegin();
    end = color.cend();
    break;
  case Buffer::ALBIDO:
    buffer_str = "albido";
    it = albido.cbegin();
    end = albido.cend();
    break;
  case Buffer::NORMAL:
    buffer_str = "normal";
    it = normal.cbegin();
    end = normal.cend();
    break;
  case Buffer::DEPTH:
    buffer_str = "depth";
    it = depth.cbegin();
    end = depth.cend();
    break;
  }

  std::string_view ext = path.substr(path.find_last_of('.') + 1);
  if (ext == "png" || ext == "bmp" || ext == "tga" || ext == "jpg") {
    std::vector<uint8_t> img(buffer_size);

    std::size_t i = 0;
    if (buffer == Buffer::NORMAL) {
      while (it != end) {
        img[i++] = static_cast<uint8_t>((*it * 0.5f + 0.5f) * 255.0f);
        ++it;
      }
    } else {
      while (it != end) {
        img[i++] = static_cast<uint8_t>(*it * 255.0f);
        ++it;
      }
    }

    if (ext == "png") {
      SPDLOG_INFO("Writting {} buffer to png file {}", buffer_str, path);
      return stbi_write_png(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                            img.data(), width * ((buffer == Buffer::DEPTH) ? 1 : 3)) != 0;
    } else if (ext == "bmp") {
      SPDLOG_INFO("Writting {} buffer to bmp file {}", buffer_str, path);
      return stbi_write_bmp(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                            img.data()) != 0;
    } else if (ext == "tga") {
      SPDLOG_INFO("Writting {} buffer to tga file {}", buffer_str, path);
      return stbi_write_tga(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                            img.data()) != 0;
    } else if (ext == "jpg") {
      SPDLOG_INFO("Writting {} buffer to jpg file {}", buffer_str, path);
      return stbi_write_jpg(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                            img.data(), 100) != 0;
    }
    return false;
  } else if (ext == "hdr") {
    std::vector<float> img(buffer_size);

    std::size_t i = 0;
    if (buffer == Buffer::NORMAL) {
      while (it != end) {
        img[i++] = *it * 0.5f + 0.5f;
        ++it;
      }
    } else {
      while (it != end) {
        img[i++] = *it;
        ++it;
      }
    }

    SPDLOG_INFO("Writting {} buffer to hdr file {}", buffer_str, path);
    return stbi_write_hdr(path.data(), width, height, (buffer == Buffer::DEPTH) ? 1 : 3,
                          img.data()) != 0;
  } else {
    SPDLOG_ERROR("Unsupported image format, only support png, bmp, tga, jpg, hdr");
    return false;
  }
}
