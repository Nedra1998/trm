# === Dependencies ===
CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 11.0.2 EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

CPMAddPackage(
  NAME spdlog
  GITHUB_REPOSITORY gabime/spdlog
  GIT_TAG "v1.14.1"
  OPTIONS "SPDLOG_FMT_EXTERNAL ON" EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

CPMAddPackage(
  NAME tracy
  GITHUB_REPOSITORY wolfpld/tracy
  GIT_TAG v0.11.0
  OPTIONS "TRACY_ENABLE ${TRM_ENABLE_PROFILING}" EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

CPMAddPackage(
  NAME argparse
  GITHUB_REPOSITORY p-ranav/argparse
  GIT_TAG v3.1 EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

CPMAddPackage(
  NAME glm
  GITHUB_REPOSITORY g-truc/glm
  GIT_TAG 1.0.1 EXCLUDE_FROM_ALL TRUE SYSTEM TRUE)

# === Options ===
set(TRM_SCENE "BOX" CACHE STRING "Scene to render")

# === Version Header ===
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
configure_file(version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.hpp)

# === Executable ===

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp" "*.c")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "*.hpp")
list(APPEND HEADERS ${CMAKE_CURRENT_BINARY_DIR}/version.hpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_executable(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE -DTRM_SCENE_NAME="${TRM_SCENE}" -DTRM_SCENE_${TRM_SCENE})
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog fmt::fmt TracyClient glm::glm
                                             argparse::argparse)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                                  ${CMAKE_CURRENT_BINARY_DIR})

