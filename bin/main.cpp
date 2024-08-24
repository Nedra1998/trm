#include <filesystem>
#include <fstream>

#include <argparse/argparse.hpp>
#include <fmt/ranges.h>
#include <nlohmann/json.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <trm/error.hpp>
#include <trm/trm.hpp>
#include <trm/version.hpp>

using json = nlohmann::json;
using namespace trm;

bool write_table(const std::string &filename,
                 const std::map<std::size_t, std::map<std::string, json>> &data, std::string xkey,
                 std::string ykey) {
  std::ofstream output(filename);
  if (!output.is_open()) {
    LOG_ERROR("Failed to open output file: {}", filename);
    return false;
  }

  std::vector<std::string> all_commits = {};
  for (const auto &it : data) {
    for (const auto &[commit, entry] : it.second) {
      if (std::find(all_commits.begin(), all_commits.end(), commit) == all_commits.end())
        all_commits.push_back(commit);
    }
  }

  output << fmt::format("{},{}", xkey, fmt::join(all_commits, ",")) << std::endl;

  for (const auto &[key, entries] : data) {
    output << key;
    for (const auto &commit : all_commits) {
      if (entries.find(commit) != entries.end()) {
        output << "," << entries.at(commit).at(ykey);
      } else {
        output << ",";
      }
    }
    output << std::endl;
  }

  output.close();
  return true;
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
    logger->set_pattern("[%H:%M:%S.%e] [%^%-7l%$]  %v");
    spdlog::set_default_logger(logger);
  }

  spdlog::cfg::load_env_levels();

  argparse::ArgumentParser program("trm", TRM_VERSION);
  program.add_argument("reference").help("Reference image path to compare against").required();
  program.add_argument("input")
      .help("Input JSON output from the renderer used to find test images")
      .required()
      .nargs(argparse::nargs_pattern::at_least_one);
  program.add_argument("-o", "--output")
      .help("CSV output file to aggregated metrics")
      .default_value(std::string{"{scene}_{metric}.csv"});
  program.add_argument("-x", "--x-key")
      .help("Key to use for the x-axis")
      .default_value(std::string{"samples"});
  program.add_argument("-y", "--y-key")
      .help("Keys to use for the y-axis")
      .default_value(std::vector<std::string>{"time", "error", "absoluteError", "squaredError",
                                              "relativeAbsoluteError", "relativeSquaredError"});

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    return 1;
  }

  LOG_INFO("TRM v{}", TRM_VERSION);
  const auto optref = Image::read(program.get<std::string>("reference"));
  if (!optref) {
    LOG_ERROR("Failed to read reference image");
    return 1;
  }
  Image reference = optref.value();

  std::map<std::size_t, std::map<std::string, json>> data;

  const auto xkey = program.get<std::string>("x-key");
  std::string scene = "";
  std::string input_path = program.get<std::string>("input");
  for (const auto input_path : program.get<std::vector<std::string>>("input")) {
    std::ifstream input_file(input_path);
    if (!input_file.is_open()) {
      LOG_ERROR("Failed to open json file: {}", input_path);
      return 1;
    }
    json input = json::parse(input_file);
    std::string commit = input["commit"];
    scene = input["scene"];
    for (const auto &entry : input["entries"]) {
      std::size_t key = entry[xkey];

      std::string filename = entry["file"];
      const auto optimg = Image::read(filename);
      if (!optimg) {
        LOG_ERROR("Failed to read image: {}", filename);
        continue;
      }
      Image img = optimg.value();
      const Error error = trm::compare(reference, img);

      data[key][commit] = json{
          {"samples", entry["samples"]},
          {"time", entry["totalTime"]},
          {"batchSize", entry["batchSize"]},
          {"batchTime", entry["batchTime"]},
          {"filename", filename},
          {"commit", commit},
          {"error", error.error},
          {"absoluteError", error.absolute},
          {"squaredError", error.squared},
          {"relativeAbsoluteError", error.relative_absolute},
          {"relativeSquaredError", error.relative_squared},
      };
    }
  }

  for (const auto &ykey : program.get<std::vector<std::string>>("y-key")) {
    std::string filename = fmt::format(fmt::runtime(program.get<std::string>("output")),
                                       fmt::arg("scene", scene), fmt::arg("metric", ykey));
    write_table(filename, data, xkey, ykey);
  }

  return 0;
}
