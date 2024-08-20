cmake_minimum_required(VERSION 3.22)

# === Project ===
project(
  "trm"
  VERSION 0.1.0
  DESCRIPTION "Tiny Ray Marcher"
  LANGUAGES C CXX)

# === Options ===
option(TRM_ENABLE_PROFILING "Enable profiling" OFF)
option(TRM_ENABLE_SANITIZERS "Enable sanitizers" OFF)

option(TRM_BUILD_DOCS "Build documentation" OFF)
option(TRM_BUILD_EXAMPLES "Build example scenes" ON)
option(TRM_BUILD_TESTS "Build unit tests" ON)

# === Dependencies ===
include(cmake/CPM.cmake)
include(cmake/GetGitRevisionDescription.cmake)
git_describe(PROJECT_VERSION_COMMIT)

# === Compiler options ===
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === Sanitizers ===

# === Targets ===
add_subdirectory(lib)

if(TRM_BUILD_TESTS)
  add_subdirectory("test")
endif()

if(TRM_BUILD_EXAMPLES)
  add_subdirectory("examples")
endif()

if(TRM_BUILD_DOCS)
  add_subdirectory("docs")
endif()

# === Packaging ===
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "TGZ;ZIP")

include(CPack)

# === Summary ===
message(
  STATUS "==========  ${PROJECT_NAME} v${PROJECT_VERSION} (${PROJECT_VERSION_COMMIT})  ==========")
message(STATUS "")
message(STATUS "System:      ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMake:       ${CMAKE_VERSION}")
message(STATUS "Generator:   ${CMAKE_GENERATOR}")
message(STATUS "Compiler:    ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build type:  ${CMAKE_BUILD_TYPE}")
message(STATUS "")
message(STATUS "Enable Profiling:    ${TRM_ENABLE_PROFILING}")
message(STATUS "Enable Sanitizers:   ${TRM_ENABLE_SANITIZERS}")
message(STATUS "")
message(STATUS "Build Unit Tests:     ${TRM_BUILD_TESTS}")
message(STATUS "Build Documentation:  ${TRM_BUILD_DOCS}")
message(STATUS "============================================")
