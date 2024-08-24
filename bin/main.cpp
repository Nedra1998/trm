#include <filesystem>
#include <fstream>

#include <argparse/argparse.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <trm/trm.hpp>
#include <trm/version.hpp>

#include "commands.hpp"

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
    logger->set_pattern("[%H:%M:%S.%e] [%^%-7l%$]  %v");
    spdlog::set_default_logger(logger);
  }

  spdlog::cfg::load_env_levels();

  argparse::ArgumentParser program("trm", TRM_VERSION);
  program.add_description("Timy Ray Marcher");

  argparse::ArgumentParser diff_command("diff");
  diff_command.add_description("Computes the difference between two images, producing an overall "
                               "metric as well as difference images");
  diff_command.add_argument("reference")
      .help("Reference image path to compare against")
      .metavar("REFERENCE")
      .required();
  diff_command.add_argument("test")
      .help("Test image path to compare against")
      .metavar("TEST")
      .required();
  diff_command.add_argument("-m", "--metric")
      .help("Metric to use for comparison")
      .choices("error", "absolute-error", "squared-error", "relative-absolute-error",
               "relative-squared-error")
      .default_value("absolute-error");
  diff_command.add_argument("-o", "--output")
      .help("Path for the output difference images")
      .default_value("diff.png");

  argparse::ArgumentParser error_command("error");
  error_command.add_description("Computes the error between two images, producing a list of error "
                                "metrics which are appended to the output CSV file");
  error_command.add_argument("reference")
      .help("Reference image path to compare against")
      .metavar("REFERENCE")
      .required();
  error_command.add_argument("data")
      .help("Path to trm output CSV data file containing images to test against")
      .metavar("DATA")
      .nargs(argparse::nargs_pattern::at_least_one)
      .required();
  error_command.add_argument("-o", "--output").help("Path for the output CSV file");

  program.add_subparser(diff_command);
  program.add_subparser(error_command);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    return 1;
  }

  LOG_INFO("TRM v{}", TRM_VERSION);

  if (program.is_subcommand_used(diff_command)) {
    return difference(diff_command);
  } else if (program.is_subcommand_used(error_command)) {
    return error(error_command);
  }

  return 0;
}
