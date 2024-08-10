#ifndef INCLUDE_SRC_IMG_HPP_
#define INCLUDE_SRC_IMG_HPP_

#include <string_view>
#include <vector>

#include "types.hpp"

namespace trm {
  enum Buffer : std::uint8_t { COLOR = 0b0001, ALBIDO = 0b0010, NORMAL = 0b0100, DEPTH = 0b1000 };

  inline Buffer operator~(Buffer a) { return (Buffer) ~(int)a; }
  inline Buffer operator|(Buffer a, Buffer b) { return (Buffer)((int)a | (int)b); }
  inline Buffer operator&(Buffer a, Buffer b) { return (Buffer)((int)a & (int)b); }
  inline Buffer operator^(Buffer a, Buffer b) { return (Buffer)((int)a ^ (int)b); }
  inline Buffer &operator|=(Buffer &a, Buffer b) { return (Buffer &)((int &)a |= (int)b); }
  inline Buffer &operator&=(Buffer &a, Buffer b) { return (Buffer &)((int &)a &= (int)b); }
  inline Buffer &operator^=(Buffer &a, Buffer b) { return (Buffer &)((int &)a ^= (int)b); }

  struct Image {
    Image(std::size_t width, std::size_t height)
        : width(width), height(height), color(width * height * 3), albido(width * height * 3),
          normal(width * height * 3), depth(width * height) {}

    std::size_t width, height;
    std::vector<Float> color;
    std::vector<Float> albido;
    std::vector<Float> normal;
    std::vector<Float> depth;

    bool write(std::string_view path, Buffer buffer);
  };
} // namespace trm

#endif // INCLUDE_SRC_IMG_HPP_
