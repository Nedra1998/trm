#include <chrono>

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
#include "types.hpp"
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

using namespace trm;

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
  program.add_argument("-r", "--resolution")
      .help("Resolution of the output impage")
      .nargs(2)
      .default_value(std::vector<std::uint32_t>{512, 512})
      .scan<'u', std::uint32_t>();
  program.add_argument("-s", "--samples")
      .help("Number of samples per pixel")
      .default_value((std::uint32_t)32)
      .scan<'u', std::uint32_t>();
  program.add_argument("-b", "--batch")
      .help("Number of samples per batch")
      .default_value((std::uint32_t)10)
      .scan<'u', uint32_t>();
  program.add_argument("-o", "--output")
      .help("Output file name pattern")
      .default_value(std::string{"output/{name}_{commit}_{samples:03}_{buffer}.png"});
  program.add_argument("--scene")
      .help("Specify the scene to render")
      .default_value(std::string{TRM_SCENE_NAME});

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    return 1;
  }

  LOG_INFO("trm v{} ({})", TRM_VERSION, TRM_VERSION_COMMIT);
#ifdef TRACY_ENABLE
  LOG_INFO("Tracy Profiling v{}.{}.{}", (int)tracy::Version::Major, (int)tracy::Version::Minor,
           (int)tracy::Version::Patch);
#endif

  const auto samples = program.get<std::uint32_t>("--samples");
  const auto batch = program.get<std::uint32_t>("--batch");
  const auto resolution = program.get<std::vector<std::uint32_t>>("--resolution");
  const auto output = program.get<std::string>("--output");
  const auto scene = program.get<std::string>("--scene");

  trm::Image img(resolution[0], resolution[1], scene);
  trm::Renderer renderer;
  renderer.camera = trm::Camera{
      .pos = {0.0f, 0.0f, -6.0f},
      .center = {0.0f, 0.0f, 0.0f},
      .up = {0.0f, 1.0f, 0.0f},
      .fov = 1.5707f,
  };

  const auto start = std::chrono::high_resolution_clock::now();
  std::size_t total_pixels = img.width * img.height * (samples / batch);
  std::atomic<std::size_t> cumulative = 0;

  renderer.progress_callback = [&](std::size_t chunk, std::size_t current, std::size_t total) {
    const auto now = std::chrono::high_resolution_clock::now();
    cumulative += chunk;
    float progress = static_cast<float>(cumulative) / static_cast<float>(total);
    float elapsed = std::chrono::duration<double>(now - start).count();
    LOG_INFO("Rendering progress report percent={:06.2f}% elapsed={:.2f}s remaining={:.2f}s",
             progress * 100, elapsed, elapsed / progress - elapsed);
  };

  LOG_INFO("Rendering Image {}x{} @ {} in {} batches", img.width, img.height, samples,
           (samples / batch));

  for (std::uint32_t cumulative_samples = 0; cumulative_samples < samples;
       cumulative_samples += batch) {
    renderer.render(img, std::min(batch, samples - cumulative_samples));
    img.write(output, trm::Buffer::ALL);
    FrameMark;
  }

  return 0;
}
