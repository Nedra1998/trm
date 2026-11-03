#include <iostream>

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  Catch::Session session;
  int returnCode = session.applyCommandLine(argc, argv);
  if (returnCode != 0)
    return returnCode;

  bool use_color = false;
  switch (session.config().defaultColourMode()) {
    case Catch::ColourMode::ANSI:
      use_color = true;
      break;
    case Catch::ColourMode::PlatformDefault:
#if defined(_WIN32) || defined(_WIN64)
      use_color = _isatty(_fileno(stdout));
#elif defined(__APPLE__) || defined(__unix__) || defined(__unix)
      use_color = ::isatty(fileno(stdout)) != 0;
#else
      use_color = false;
#endif
      break;
    default:
      use_color = false;
      break;
  }

  for (auto &reporter : session.configData().reporterSpecifications) {
    std::string name = reporter.name();
    std::transform(name.begin(), name.end(), name.begin(), [](char c) { return std::tolower(c); });
    if (name == "xml" || name == "sonarqube" || name == "junit") {
      use_color = false;
      break;
    }
  }

  if (use_color) {
    auto logger = spdlog::stdout_color_mt("trm");
    logger->set_pattern("[\033[90m%H:%M:%S.%e\033[0m] [%^%l%$]  %v");
    spdlog::set_default_logger(logger);
  } else {
    auto logger = std::make_shared<spdlog::logger>(
        "trm", std::make_shared<spdlog::sinks::ostream_sink_mt>(std::cout));
    spdlog::set_default_logger(logger);
  }

  switch (session.config().verbosity()) {
    case Catch::Verbosity::Quiet:
      spdlog::set_level(spdlog::level::err);
      break;
    case Catch::Verbosity::Normal:
      spdlog::set_level(spdlog::level::info);
      break;
    case Catch::Verbosity::High:
      spdlog::set_level(spdlog::level::trace);
      break;
  }

  return session.run();
}

uint32_t factorial(uint32_t number) {
  return number <= 1 ? number : factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]") {
  REQUIRE(factorial(1) == 1);
  REQUIRE(factorial(2) == 2);
  REQUIRE(factorial(3) == 6);
  REQUIRE(factorial(10) == 3'628'800);
}
