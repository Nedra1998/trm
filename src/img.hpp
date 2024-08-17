#ifndef INCLUDE_SRC_IMG_HPP_
#define INCLUDE_SRC_IMG_HPP_

#include <string_view>
#include <vector>

#include "types.hpp"

namespace trm {
  enum Buffer : std::uint8_t {
    COLOR = 0b0001,
    ALBEDO = 0b0010,
    NORMAL = 0b0100,
    DEPTH = 0b1000,
    ALL = 0b1111
  };

  inline Buffer operator~(Buffer a) { return (Buffer) ~(int)a; }
  inline Buffer operator|(Buffer a, Buffer b) { return (Buffer)((int)a | (int)b); }
  inline Buffer operator&(Buffer a, Buffer b) { return (Buffer)((int)a & (int)b); }
  inline Buffer operator^(Buffer a, Buffer b) { return (Buffer)((int)a ^ (int)b); }
  inline Buffer &operator|=(Buffer &a, Buffer b) { return (Buffer &)((int &)a |= (int)b); }
  inline Buffer &operator&=(Buffer &a, Buffer b) { return (Buffer &)((int &)a &= (int)b); }
  inline Buffer &operator^=(Buffer &a, Buffer b) { return (Buffer &)((int &)a ^= (int)b); }

  class Image {
  public:
    Image(std::size_t width, std::size_t height, std::string_view name = std::string_view(),
          std::size_t samples = 0)
        : width(width), height(height), samples(samples), name(name), color(width * height * 3),
          albedo(width * height * 3), normal(width * height * 3), depth(width * height) {}

    std::size_t width, height, samples;
    std::string_view name;
    std::vector<Float> color;
    std::vector<Float> albedo;
    std::vector<Float> normal;
    std::vector<Float> depth;

    bool write(std::string_view path, Buffer buffer) const;

  private:
    bool write_u8(std::string_view path, std::string_view ext, Buffer buffer) const;
    bool write_f32(std::string_view path, std::string_view ext, Buffer buffer) const;
  };
} // namespace trm

#endif // INCLUDE_SRC_IMG_HPP_
