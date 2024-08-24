#include "scene.hpp"

std::tuple<trm::Float, std::uint32_t> trm::Scene::sdf(const Vec3 &p) const {
  Float dist = std::numeric_limits<Float>::max();
  std::uint32_t obj = 0;
  for (std::size_t i = 0; i < sdfs_.size(); ++i) {
    Float d = std::fabs(sdfs_[i](inv_transforms_[i] * Vec4(p, 1.0f)));
    if (d < dist) {
      dist = d;
      obj = i + 1;
    }
  }
  return {dist, obj};
}

trm::Vec3 trm::Scene::normal(const Vec3 &p, std::uint32_t obj) const {
  const auto &func = sdfs_[obj - 1];
  Vec3 op = inv_transforms_[obj - 1] * Vec4(p, 1.0f);
  return normalize(
      transforms_[obj - 1] *
      Vec4(func(Vec3(op.x + 0.0001f, op.y, op.z)) - func(Vec3(op.x - 0.0001f, op.y, op.z)),
           func(Vec3(op.x, op.y + 0.0001f, op.z)) - func(Vec3(op.x, op.y - 0.0001f, op.z)),
           func(Vec3(op.x, op.y, op.z + 0.0001f)) - func(Vec3(op.x, op.y, op.z - 0.0001f)), 0.0f));
}
