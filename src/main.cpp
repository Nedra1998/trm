#include <argparse/argparse.hpp>
#include <common/TracyVersion.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>

#include "img.hpp"
#include "log.hpp"
#include "render.hpp"
#include "version.hpp"

void *operator new(std ::size_t count) {
  auto ptr = malloc(count);
  TracyAlloc(ptr, count);
  return ptr;
}
void operator delete(void *ptr) noexcept {
  TracyFree(ptr);
  free(ptr);
}

int main(int argc, char *argv[]) {

#if defined(_WIN32) || defined(_WIN64)
  const bool is_tty = _isatty(_fileno(stderr));
#elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
  const bool is_tty = isatty(fileno(stderr)) != 0;
#else
  const bool is_tty = false;
#endif

  if (is_tty) {
    auto logger = spdlog::stderr_color_mt("trm");
    logger->set_pattern("[\033[90m%H:%M:%S.%e\033[0m] [%^%-7l%$]  %v");
    spdlog::set_default_logger(logger);
  } else {
    auto logger = spdlog::stderr_logger_mt("trm");
    spdlog::set_default_logger(logger);
  }
  spdlog::cfg::load_env_levels();

  argparse::ArgumentParser program("trm", TRM_VERSION);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    return 1;
  }

  LOG_INFO("trm v{}", TRM_VERSION);
#ifdef TRACY_ENABLE
  LOG_INFO("Tracy Profiling v{}.{}.{}", (int)tracy::Version::Major, (int)tracy::Version::Minor,
           (int)tracy::Version::Patch);
#endif

  trm::Image img(1024, 1024);
  trm::Renderer renderer;
  renderer.camera = trm::Camera{
      .pos = {0.0f, 0.0f, -6.0f},
      .center = {0.0f, 0.0f, 0.0f},
      .up = {0.0f, 1.0f, 0.0f},
      .fov = 1.5707f,
  };

  renderer.render(img, 256);
  img.write("out/{buffer}.{samples:03}.png", trm::Buffer::ALL);

  // for (std::size_t samples = 0; samples < 10; ++samples) {
  //   renderer.render(img, max(2, (1 << samples)));
  //   img.write("out/{buffer}.{samples:03}.png", trm::Buffer::ALL);
  //   FrameMark;
  // }

  return 0;
}
