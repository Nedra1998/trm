cmake_minimum_required(VERSION 3.22)

# === Project ===
project(
    "trm"
    VERSION 0.1.0
    DESCRIPTION "Tiny Ray Marcher"
    LANGUAGES CXX
)

# === Options ===
option(TMP_ENABLE_PROFILING "Enable profiling" OFF)
option(TMP_ENABLE_SANITIZERS "Enable sanitizers" OFF)

option(TMP_BUILD_TESTS "Build tests" ON)
option(TMP_BUILD_DOCS "Build documentation" OFF)

# === Dependencies ===
include(cmake/CPM.cmake)

# === Compiler options ===
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === Sanitizers ===

# === Targets ===
add_subdirectory("src")

if(TMP_BUILD_TESTS)
  enable_testing()
  add_subdirectory("tst")
endif()

if(TMP_BUILD_DOCS)
  add_subdirectory("doc")
endif()

# === Packaging ===
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "TGZ;ZIP")

include(CPack)

# === Summary ===
message(STATUS "==========  ${PROJECT_NAME} v${PROJECT_VERSION} ==========")
message(STATUS "")
message(STATUS "Configuration:")
message(STATUS "  CMake: ${CMAKE_VERSION}")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "  Binary directory: ${CMAKE_BINARY_DIR}")
message(STATUS "")
message(STATUS "Features:")
message(STATUS "  Profiling enabled: ${TMP_ENABLE_PROFILING}")
message(STATUS "  Sanitizers enabled: ${TMP_ENABLE_SANITIZERS}")
message(STATUS "")
message(STATUS "Targets:")
message(STATUS "  Build tests: ${TMP_BUILD_TESTS}")
message(STATUS "  Build documentation: ${TMP_BUILD_DOCS}")
