/**
 * @file image.hpp
 * @brief Image handling for TRM
 *
 * This file provides an interface for working with images that is used throughout TRM.
 */

#ifndef TRM_IMAGE_HPP_
#define TRM_IMAGE_HPP_

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include <fmt/base.h>

#include "trm/float.hpp"

namespace trm {

  /**
   * @brief The buffer type for an image
   *
   * This enum represents the different types of buffers that an image can have. The buffer types
   * will influence how the image is normalized prior to writing to disk.
   */
  enum struct Buffer : std::uint8_t {
    // clang-format off
    NONE   = 0b0000, ///< No specific buffer
    COLOR  = 0b0001, ///< Color buffer (RGB)
    ALBEDO = 0b0010, ///< Albedo buffer (RGB)
    NORMAL = 0b0100, ///< Normal buffer (XYZ)
    DEPTH  = 0b1000, ///< Depth buffer (Z)
    ALL    = 0b1111, ///< All buffers
    // clang-format on
  };

  // clang-format off
  inline Buffer operator~(Buffer a) { return (Buffer) ~(std::uint8_t)a; }
  inline Buffer operator|(Buffer a, Buffer b) { return (Buffer)((std::uint8_t)a | (std::uint8_t)b); }
  inline Buffer operator&(Buffer a, Buffer b) { return (Buffer)((std::uint8_t)a & (std::uint8_t)b); }
  inline Buffer operator^(Buffer a, Buffer b) { return (Buffer)((std::uint8_t)a ^ (std::uint8_t)b); }
  inline Buffer &operator|=(Buffer &a, Buffer b) { return (Buffer &)((std::uint8_t &)a |= (std::uint8_t)b); }
  inline Buffer &operator&=(Buffer &a, Buffer b) { return (Buffer &)((std::uint8_t &)a &= (std::uint8_t)b); }
  inline Buffer &operator^=(Buffer &a, Buffer b) { return (Buffer &)((std::uint8_t &)a ^= (std::uint8_t)b); }
  // clang-format on

  /**
   * @class Image
   * @brief An image representation
   *
   * This image class represents an image that can be read from or written to disk. It can be
   * constructed with a specific buffer type, or it can be read from disk. The image can be written
   * to disk in a variety of formats.
   */
  class Image {
  public:
    /**
     * @brief Construct a new Image object with the given width, height, and depth
     *
     * @param width Width in pixels of the image.
     * @param height Height in pixels of the image.
     * @param depth Number of channels in the image.
     */
    Image(std::uint32_t width, std::uint32_t height, std::uint32_t depth = 3)
        : width(width), height(height), depth(depth), buffer(Buffer::NONE),
          data(width * height * depth) {}
    /**
     * @brief Construct a new Image object with the given width, height, and buffer type
     *
     * @param width Width of the image in pixels.
     * @param height Height of the image in pixels.
     * @param buffer The buffer type for the image, implies a depth of 1 for `Buffer::DEPTH` and 3
     * otherwise.
     */
    Image(std::uint32_t width, std::uint32_t height, Buffer buffer)
        : width(width), height(height), depth(buffer == Buffer::DEPTH ? 1 : 3), buffer(buffer),
          data(width * height * (buffer == Buffer::DEPTH ? 1 : 3)) {}

    /**
     * @brief Read an image from disk
     *
     * The supported image formats are those supported by the `stb_image` library.
     * - JPEG
     * - PNG
     * - TGA
     * - BMP
     * - PSD
     * - HDR
     * - PNM (PPM and PGM)
     *
     * @param path The path to the image file to read.
     * @return An optional containing the image if it was read successfully, or an empty optional if
     * an error occurred.
     */
    static std::optional<Image> read(std::string_view path);

    /**
     * @brief Write the image to disk.
     *
     * The supported image formats are those supported by the `stb_image_write` library. The format
     * is determined by the file extension of the path. The supported formats are:
     * - JPEG
     * - PNG
     * - TGA
     * - BMP
     * - HDR
     *
     * @param path The file path to write the image to.
     * @return `true` if the image was written successfully, `false` otherwise.
     */
    bool write(std::string_view path) const;

    /**
     * @brief Normalize the image data according to the buffer type.
     *
     * This function will normalize the image data according to the buffer type. The normalization
     * will not modify the original data, but will return a new vector with the normalized data.
     *
     * Here is how the normalization is performed for each buffer type:
     * - `Buffer::COLOR`: Clamp each color channel to the range `[0, 1]`.
     * - `Buffer::ALBEDO`: Clamp each albedo channel to the range `[0, 1]`.
     * - `Buffer::NORMAL`: Map each normal component from the range `[-1, 1]` to `[0, 1]`.
     * - `Buffer::DEPTH`: Map the depth from the range `[0, maximum]` to `[0, 1]`.
     *
     * @return A vector containing the normalized.
     */
    std::vector<Float> normalized() const;

    /**
     * @brief Normalize the image data in place.
     *
     * This function will normalize the image data according to the buffer type. The normalization
     * will modify the original data in place.
     *
     * @see normalized
     */
    inline void normalize() { data = normalized(); }

    /**
     * @brief Compute the MD5 hash of the image data.
     *
     * @return The MD5 hash of the image data as a hex string.
     */
    std::string md5() const;

    inline void set(std::size_t i, const Float &value) {
      if (width == 0 || height == 0)
        return;
      data[i * depth] += value;
    }
    inline void set(std::size_t i, const Vec3 &value) {
      if (width == 0 || height == 0)
        return;
      data[i * depth + 0] += value.r;
      data[i * depth + 1] += value.g;
      data[i * depth + 2] += value.b;
    }

    inline Float &operator[](std::size_t index) { return data[index]; }

    inline Image &operator*=(const Float scalar) {
      for (auto &d : data) {
        d *= scalar;
      }
      return *this;
    }

    std::uint32_t width, height, depth;
    Buffer buffer;
    std::vector<Float> data;
  };

  struct ImageSet {
    ImageSet(std::uint32_t width, std::uint32_t height, Buffer buffers = Buffer::ALL);

    inline ImageSet *operator*=(const Float scalar) {
      color *= scalar;
      albedo *= scalar;
      normal *= scalar;
      depth *= scalar;
      return this;
    }

    std::uint32_t width, height, samples = 0;
    Image color, albedo, normal, depth;

    std::string fmt(std::string_view pattern, Buffer buffer) const;
    bool write(std::string_view pattern, Buffer buffer) const;
  };
} // namespace trm

template <> struct fmt::formatter<trm::Buffer> : fmt::formatter<string_view> {
  auto format(trm::Buffer buf, format_context &ctx) const -> format_context::iterator;
};

#endif // TRM_IMAGE_HPP_
