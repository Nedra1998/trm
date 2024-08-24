#include <fstream>

#include <argparse/argparse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <trm/trm.hpp>
#include <trm/version.hpp>

using json = nlohmann::json;
using namespace trm;

void scene(Renderer &renderer);

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
    logger->set_pattern("[%H:%M:%S.%e] [%^%-7l%$]  %v");
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
      .default_value((std::uint32_t)8)
      .scan<'u', uint32_t>();
  program.add_argument("-o", "--output")
      .help("Output file name pattern")
      .default_value(std::string{"output/" EXAMPLE_NAME "_{commit}_{buffer}_{samples:03d}.png"});
  program.add_argument("-j", "--json")
      .help("Output file name pattern for the json output file")
      .default_value(std::string{"output/" EXAMPLE_NAME "_{commit}.json"});

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    return 1;
  }

  LOG_INFO("TRM v{}", TRM_VERSION);

  Renderer renderer;
  renderer.cfg = Config{.max_depth = 16,
                        .max_steps = 256,
                        .max_distance = 1e3f,
                        .epsilon = 1e-6f,
                        .bias = 1e-5f,
                        .features = Feature::PROGRESS_REPORT};

  scene(renderer);

  const auto resolution = program.get<std::vector<std::uint32_t>>("--resolution");

  ImageSet img(resolution[0], resolution[1]);
  const auto result = renderer.render(program.get<std::string>("--output"), img,
                                      program.get<std::uint32_t>("--samples"),
                                      program.get<std::uint32_t>("--batch"), Buffer::COLOR);

  std::string output = fmt::format(fmt::runtime(program.get<std::string>("--json")),
                                   fmt::arg("commit", TRM_VERSION_COMMIT));
  std::ofstream out(output);
  json report = json::object();
  report["commit"] = TRM_VERSION_COMMIT;
  report["scene"] = EXAMPLE_NAME;

  for (const auto &step : result) {
    report["entries"].push_back({
        {"samples", step.samples},
        {"batchSize", step.batch_size},
        {"file", step.filename},
        {"totalTime", step.total_time.count()},
        {"batchTime", step.batch_time.count()},
    });
  }

  if (out.is_open()) {
    out << report.dump() << std::endl;
  } else {
    LOG_ERROR("Failed to open output file: {}", output);
  }

  return 0;
}
