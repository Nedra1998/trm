# === Dependencies ===
CPMAddPackage(
  NAME argparse
  GITHUB_REPOSITORY p-ranav/argparse
  GIT_TAG v3.1 EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

CPMAddPackage(
  NAME csv2
  GITHUB_REPOSITORY p-ranav/csv2
  GIT_TAG master EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

CPMAddPackage(
  NAME json
  GITHUB_REPOSITORY nlohmann/json
  GIT_TAG v3.11.3 EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

# === Executable ===
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp" "*.c")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "*.hpp")

add_executable(bin ${SOURCES} ${HEADERS})
add_executable(${PROJECT_NAME}::bin ALIAS bin)
set_target_properties(bin PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(bin PUBLIC ${PROJECT_NAME}::${PROJECT_NAME} argparse::argparse nlohmann_json::nlohmann_json csv2::csv2)
target_include_directories(bin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)
