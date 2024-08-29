/**
 * @file renderer.hpp
 * @brief Core rendering methods for TRM.
 */

#ifndef TRM_RENDERER_HPP_
#define TRM_RENDERER_HPP_

#include <chrono>

#include "trm/config.hpp"
#include "trm/image.hpp"
#include "trm/scene.hpp"

namespace trm {
  /**
   * @class Ray
   * @brief A ray in 3D space, represented by an origin and direction.
   */
  struct Ray {
    /**
     * @brief Create a new Ray object.
     *
     * @param o The origin of the ray.
     * @param d The direction of the ray.
     * @param mat The material index of the medium the ray is in.
     */
    Ray(const Vec3 &o, const Vec3 &d, const std::uint32_t mat = 0)
        : o(o), d(normalize(d)), material(mat) {}
    Vec3 o;                 ///< Origin of the ray.
    Vec3 d;                 ///< Direction of the ray.
    std::uint32_t material; ///< Material index of the medium the ray is in.
  };

  /**
   * @class Camera
   * @brief Camera representation for the renderer.
   */
  struct Camera {
    Vec3 pos;    ///< Position of the camera.
    Vec3 center; ///< Point the camera is looking at.
    Vec3 up;     ///< Up vector of the camera.
    Float fov;   ///< Field of view of the camera.
  };

  /**
   * @class Renderer
   * @brief Core ray marching renderer.
   */
  class Renderer {
  public:
    /**
     * @class Pixel
     * @brief Representation of a pixel buffers.
     */
    struct Pixel {
      Vec3 color = Vec3(0.0f);  ///< Color of the pixel.
      Vec3 albedo = Vec3(0.0f); ///< Albedo of the pixel.
      Vec3 normal = Vec3(0.0f); ///< Normal of the pixel.
      Float depth = 0.0f;       ///< Depth of the pixel.

      /**
       * @brief Combine the values of two pixels.
       *
       * @param rhs The pixel to add to this pixel.
       * @return The combined pixel.
       */
      inline Pixel &operator+=(const Pixel &rhs) {
        color += rhs.color;
        albedo += rhs.albedo;
        normal += rhs.normal;
        depth += rhs.depth;
        return *this;
      }

      /**
       * @brief Scale the values of the pixel by a scalar.
       *
       * @param scalar Scalar to multiply the pixel by.
       * @return The pixel scaled by the scalar.
       */
      inline Pixel &operator*=(const Float scalar) {
        color *= scalar;
        albedo *= scalar;
        normal *= scalar;
        depth *= scalar;
        return *this;
      }
    };

    /**
     * @class RenderStats
     * @brief Statistics recorded during the rendering process.
     */
    struct RenderStats {
      std::string filename;                 ///< Filename the snapshot image was saved to.
      std::chrono::milliseconds total_time; ///< Total time taken to render the image so far.
      std::chrono::milliseconds batch_time; ///< Time taken to render the current snapshot batch.
      std::uint32_t samples;                ///< Total number of samples rendered so far.
      std::uint32_t batch_size; ///< The number of samples within the current snapshot batch.
    };

    Config cfg;                       ///< Configuration for the renderer.
    Camera camera;                    ///< Camera used to render the scene.
    Scene scene;                      ///< The scene to render.
    std::uint32_t target_samples = 0; ///< Target number of samples to render.

    /**
     * @brief Progressive rendering of the scene, saving snapshots at fixed intervals.
     *
     * @param filename The filename pattern to save the snapshots to.
     * @param img The image set to render to.
     * @param samples The total number of samples to render.
     * @param batch_size The number of samples to include in each batch.
     * @param buffers The image buffers to write out in each snapshot.
     *
     * @return A vector of statistics recorded during the rendering process.
     */
    std::vector<RenderStats> render(std::string_view filename, ImageSet &img, std::uint32_t samples,
                                    std::uint32_t batch_size, Buffer buffers = Buffer::ALL);
    /**
     * @brief Render the scene to an image.
     *
     * @param img The image set to render to.
     * @param samples The number of samples to render for each pixel.
     * @return The time taken to render the image.
     */
    std::chrono::milliseconds render(ImageSet &img, std::uint32_t samples) const;

  private:
    /**
     * @brief Trace a ray through the scene and return the pixel values.
     *
     * This is the core method of the renderer. It uses `ray_march` to find the closest ray-object
     * intersection. Then depending on the material of the object it computes the
     * reflected/refracted direction, and recursivly calls itself to trace the reflected/refracted
     * ray. With the result of the recursive call, it computes the color, albedo, normal, and depth
     * of the pixel at the intersection point, accounting for the BSDF of the material.
     *
     * @param r The ray to trace through the scene.
     * @param depth The recursion depth of the tracing.
     * @return The pixel values of color, albedo, normal, and depth along the ray.
     */
    Pixel trace(const Ray &r, const std::size_t depth = 0) const;

    /**
     * @brief Process ray marching to find the closest intersection point.
     *
     * The process of ray marching involves stepping along the ray in small increments and
     * evaluating the signed distance field (SDF) of the scene at each step. The SDF is a function
     * that returns the distance to the closest object in the scene. By evaluating the SDF at each
     * step, we can find the closest object and the distance to it. If the distance is less than a
     * configured threshold, we consider the object to be hit by the ray. If no object is within
     * that threshold, we advance the ray and repeate the process.
     *
     * @param r The ray to march through the scene.
     * @return A tuple containing the distance to the closest object and the index of the object, or
     * `0` if no object was hit.
     */
    std::tuple<Float, std::uint32_t> ray_march(const Ray &r) const;
  };
} // namespace trm

#endif // TRM_RENDERER_HPP_
