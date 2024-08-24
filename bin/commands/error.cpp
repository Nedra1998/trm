#include <filesystem>
#include <fstream>
#include <string>

#include <fmt/std.h>
#include <trm/error.hpp>
#include <trm/image.hpp>
#include <trm/log.hpp>

#include "commands.hpp"

struct Entry {
  std::string commit, file, seed;
  std::uint32_t samples, time;
  double error = 0.0, absolute = 0.0, squared = 0.0, relative_absolute = 0.0,
         relative_squared = 0.0, delta_e = 0.0;
};

std::vector<Entry> process(const std::filesystem::path &datapath, const trm::Image &ref) {
  std::error_code ec;
  if (!std::filesystem::exists(datapath, ec)) {
    LOG_ERROR("Data path '{}' does not exist: {}", datapath, ec.message());
    return {};
  }

  std::ifstream datafile(datapath);
  if (!datafile.is_open()) {
    LOG_ERROR("Failed to open data file '{}'", datapath);
    return {};
  }

  // Skip the header row from the csv
  datafile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  std::vector<Entry> entries;
  while (datafile.good()) {
    Entry entry;
    std::getline(datafile, entry.commit, ',');
    if (entry.commit.empty()) {
      break;
    }

    std::getline(datafile, entry.seed, ',');
    std::getline(datafile, entry.file, ',');

    std::string samples_str, time_str;
    std::getline(datafile, samples_str, ',');
    std::getline(datafile, time_str, '\n');
    entry.samples = std::stoul(samples_str);
    entry.time = std::stoul(time_str);
    entries.push_back(entry);
  }

  for (auto &entry : entries) {
    std::filesystem::path testpath = entry.file;
    if (!std::filesystem::exists(testpath, ec)) {
      LOG_ERROR("Test image path '{}' does not exist: {}", testpath, ec.message());
      continue;
    }
    const std::optional<trm::Image> testopt = trm::Image::read(testpath.string());
    if (!testopt) {
      LOG_ERROR("Failed to read test image '{}'", testpath);
      continue;
    }
    const trm::Image &test = *testopt;
    const trm::Error error = trm::compare(ref, test);
    entry.error = error.error;
    entry.absolute = error.absolute;
    entry.squared = error.squared;
    entry.relative_absolute = error.relative_absolute;
    entry.relative_squared = error.relative_squared;
    entry.delta_e = error.delta_e;
  }
  return entries;
}

int error(argparse::ArgumentParser &parser) {
  std::filesystem::path refpath = parser.get<std::string>("reference");

  std::error_code ec;
  if (!std::filesystem::exists(refpath, ec)) {
    LOG_ERROR("Reference image path '{}' does not exist: {}", refpath, ec.message());
    return 1;
  }

  const std::optional<trm::Image> refopt = trm::Image::read(refpath.string());
  if (!refopt) {
    LOG_ERROR("Failed to read reference image '{}'", refpath);
    return 1;
  }

  const trm::Image &ref = *refopt;

  std::vector<Entry> entries;
  for (const auto &datapath : parser.get<std::vector<std::string>>("data")) {
    std::vector<Entry> new_entries = process(datapath, ref);
    entries.insert(entries.end(), new_entries.begin(), new_entries.end());
  }

  std::streambuf *buf;
  std::ofstream outfile;
  if (parser.is_used("--output")) {
    std::filesystem::path output = fmt::format(fmt::runtime(parser.get<std::string>("--output")));
    if (!std::filesystem::is_directory(output.parent_path())) {
      LOG_DEBUG("Creating directory: {}", output.parent_path().string());
      std::filesystem::create_directories(output.parent_path());
    }
    outfile.open(output);
    buf = outfile.rdbuf();
  } else {
    buf = std::cout.rdbuf();
  }
  std::ostream out(buf);

  std::print(out, "commit,seed,filename,samples,time,error,absolute,squared,relative_absolute,"
                  "relative_squared,cie76\n");
  for (const auto &entry : entries) {
    std::print(out,
               "{},{},{},{},{},{:.5f},{:.5f},{:.5f},"
               "{:.5f},{:.5f},{:.5f}\n",
               entry.commit, entry.seed, entry.file, entry.samples, entry.time, entry.error,
               entry.absolute, entry.squared, entry.relative_absolute, entry.relative_squared,
               entry.delta_e);
  }
  return 0;
}
