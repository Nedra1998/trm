#include <argparse/argparse.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <version.hpp>

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
    logger->set_pattern("[\033[90m%Y-%m-%d %H:%M:%S.%e\033[0m] [\033[36m%n\033[0m] [%^%l%$] "
                        "[\033[35m%s:%#\033[0m] %v");
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

  SPDLOG_INFO("trm v{}", TRM_VERSION);

  return 0;
}
