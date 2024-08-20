#ifndef TRM_SCENE_HPP_
#define TRM_SCENE_HPP_

#include <functional>
#include <vector>

#include "material.hpp"

namespace trm {
  class Scene {
  public:
    std::tuple<Float, std::uint32_t> sdf(const Vec3 &p) const;
    Vec3 normal(const Vec3 &p, std::uint32_t obj) const;
    inline Material materials(std::uint32_t obj) const { return materials_[obj - 1]; }

    std::vector<Mat4> transforms_;
    std::vector<Mat4> inv_transforms_;
    std::vector<Material> materials_;
    std::vector<std::function<Float(Vec3)>> sdfs_;
  };
} // namespace trm

#endif // TRM_SCENE_HPP_
