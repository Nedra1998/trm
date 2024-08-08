# === Dependencies ===
CPMAddPackage(
  NAME spdlog
  GITHUB_REPOSITORY gabime/spdlog
  GIT_TAG "v1.14.1"
  OPTIONS "SPDLOG_USE_STD_FORMAT ON"
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE
)

CPMAddPackage(
  NAME tracy
  GITHUB_REPOSITORY wolfpld/tracy
  GIT_TAG v0.11.0
  OPTIONS "TRACY_ENABLE ${TRM_ENABLE_PROFILING}"
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE
)

CPMAddPackage(
  NAME argparse
  GITHUB_REPOSITORY p-ranav/argparse
  GIT_TAG v3.1
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE
)

# === Version Header ===
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
configure_file(version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.hpp)

# === Library ===

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "*.hpp")
list(FILTER SOURCES EXCLUDE REGEX "main.cpp")
list(APPEND HEADERS ${CMAKE_CURRENT_BINARY_DIR}/version.hpp)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog TracyClient)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# === Executable ===
file(GLOB_RECURSE EXEC_SOURCES CONFIGURE_DEPENDS "main.cpp")
add_executable(${PROJECT_NAME}-bin ${EXEC_SOURCES})
set_target_properties(${PROJECT_NAME}-bin PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}-bin PRIVATE ${PROJECT_NAME}::${PROJECT_NAME} argparse)
