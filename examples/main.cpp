#include <filesystem>
#include <fstream>

#include <argparse/argparse.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <trm/trm.hpp>

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

  std::srand(std::time(nullptr));
  argparse::ArgumentParser program("trm", TRM_VERSION);
  program.add_argument("-r", "--resolution")
      .help("Resolution of the output impage")
      .nargs(2)
      .default_value(std::vector<std::uint32_t>{512, 512})
      .scan<'u', std::uint32_t>();
  program.add_argument("-s", "--samples")
      .help("Number of samples per pixel")
      .default_value((std::uint32_t)64)
      .scan<'u', std::uint32_t>();
  program.add_argument("-b", "--batch")
      .help("Number of samples per batch")
      .default_value((std::uint32_t)8)
      .scan<'u', uint32_t>();
  program.add_argument("-S", "--seed")
      .help("Seed for the random number generator")
      .default_value((std::uint32_t)std::rand())
      .scan<'x', uint32_t>();
  program.add_argument("-o", "--output")
      .help("Output file name pattern")
      .default_value(std::string{"output/" EXAMPLE_NAME "_{commit}_{seed:08x}_{samples:03d}.png"});
  program.add_argument("-d", "--data")
      .help("Output file name pattern for the datafile")
      .default_value(std::string{"output/" EXAMPLE_NAME ".csv"});

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    return 1;
  }

  trm::srand(program.get<std::uint32_t>("--seed"));
  LOG_INFO("TRM v{} (seed={:08x})", TRM_VERSION, trm::seed);

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

  std::filesystem::path output = fmt::format(fmt::runtime(program.get<std::string>("--data")));
  bool write_header = false;
  if (!std::filesystem::exists(output)) {
    if (!std::filesystem::is_directory(output.parent_path())) {
      LOG_DEBUG("Creating directory: {}", output.parent_path().string());
      std::filesystem::create_directories(output.parent_path());
    }
    write_header = true;
  }
  std::ofstream out(output, std::ios::app);
  if (write_header) {
    std::print(out, "commit,seed,filename,samples,time\n");
  }

  if (out.is_open()) {
    for (const auto &step : result) {
      std::print(out, "{},{:08x},{},{},{}\n", TRM_VERSION_COMMIT, trm::seed, step.filename,
                 step.samples, step.total_time.count());
    }
  } else {
    LOG_ERROR("Failed to open output file: {}", output.string());
  }
  return 0;
}
