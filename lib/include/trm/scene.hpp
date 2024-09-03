/**
 * @file scene.hpp
 * @brief Scene object used by the renderer.
 */

#ifndef TRM_SCENE_HPP_
#define TRM_SCENE_HPP_

#include <functional>
#include <vector>

#include "material.hpp"

namespace trm {
  /**
   * @class Scene
   * @brief The scene used by the renderer.
   *
   * The scene object contains all of the objects in the scene, their materials, and their
   * transformations. It acts as the interface to evaluate the scene's signed distance field (SDF).
   * The SDF is a function that returns the distance to the closest object in the scene and the
   * object index. The object index is used to retrieve the object's material.
   *
   * It also provides interfaces to determine the object normal at a given point.
   */
  class Scene {
  public:
    /**
     * @brief Compute the signed distance field (SDF) of the scene at a given point.
     *
     * @param p The point to evaluate the SDF at.
     * @return A tuple containing the distance to the closest object and the index of the object;
     */
    std::tuple<Float, std::uint32_t> sdf(const Vec3 &p) const;

    /**
     * @brief Estimate the normal of the object at a given point.
     *
     * One of the issues with ray marching is the computation of a normal. There isn't a direct way
     * to compute the normal of an implicit surface. However, we can estimate the normal by using
     * the gradient of the SDF. The gradient of the SDF is the normal of the surface at that point.
     * This is an approximation and is not always accurate, but it is good enough for most cases.
     *
     * @param p The point to estimate the normal at.
     * @param obj Object index to estimate the normal for.
     * @return A normal vector for the object at the given point.
     */
    Vec3 normal(const Vec3 &p, std::uint32_t obj) const;
    /**
     * @brief Get the object's material.
     *
     * @param obj Object index to get the material for.
     * @return Material of the object.
     */
    inline const Material &materials(std::uint32_t obj) const { return materials_[obj - 1]; }

    Vec3 sky = Vec3(0.0f);             ///< Color of the sky.
    std::vector<Mat4> transforms_;     ///< Transformations of the objects in the scene.
    std::vector<Mat4> inv_transforms_; ///< Inverse transformations of the objects in the scene.
    std::vector<Material> materials_;  ///< Materials of the objects in the scene.
    std::vector<std::function<Float(Vec3)>> sdfs_; ///< Signed distance functions of the objects.
  };
} // namespace trm

#endif // TRM_SCENE_HPP_
