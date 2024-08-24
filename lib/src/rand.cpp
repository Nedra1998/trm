#include "rand.hpp"

#include <random>

namespace trm {
  std::uint32_t seed = rd();
  std::mt19937 gen(seed);
} // namespace trm
