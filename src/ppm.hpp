#ifndef PPM_HPP_
#define PPM_HPP_

#include <string_view>

namespace trm::ppm {
  /**
   * @brief Write a PPM image to a file.
   *
   * Currently only supports writting P3 or P6 PPM images, with the option to write in binary
   * format. Other PPM formats are not supported at this time.
   *
   * @param path Path to the output file.
   * @param width Width of the image in pixels.
   * @param height Height of the image in pixels.
   * @param data Float array of size width * height * 3 containing the image data.
   * @param binary Write the image in binary format.
   * @return `true` if the image was written successfully, `false` otherwise.
   */
  bool write(std::string_view path, std::size_t width, std::size_t height, float *data,
             bool binary = true);
} // namespace trm::ppm

#endif // PPM_HPP_
