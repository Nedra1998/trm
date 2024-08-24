# === Dependencies ===
CPMAddPackage(
  NAME argparse
  GITHUB_REPOSITORY p-ranav/argparse
  GIT_TAG v3.1 EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

CPMAddPackage(
  NAME json
  GITHUB_REPOSITORY nlohmann/json
  GIT_TAG v3.11.3 EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

# === Executable ===
file(GLOB_RECURSE EXAMPLES CONFIGURE_DEPENDS *.cpp)
list(FILTER EXAMPLES EXCLUDE REGEX ".*main.cpp")

foreach(file ${EXAMPLES})
  get_filename_component(file_name ${file} NAME_WE)
  add_executable(example-${file_name} main.cpp ${file})
  target_compile_definitions(example-${file_name} PRIVATE EXAMPLE_NAME="${file_name}")
  target_link_libraries(example-${file_name} ${PROJECT_NAME}::${PROJECT_NAME} argparse::argparse nlohmann_json::nlohmann_json)
  install(TARGETS example-${file_name} DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()
