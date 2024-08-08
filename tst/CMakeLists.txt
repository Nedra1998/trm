# === Dependencies ===
CPMAddPackage(
  NAME Catch
  GITHUB_REPOSITORY catchorg/Catch2
  GIT_TAG v3.5.0
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE
)

list(APPEND CMAKE_MODULE_PATH ${Catch_SOURCE_DIR}/extras)
include(Catch)

# === Executable ===

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "*.h")

add_executable(unit-tests ${SOURCES} ${HEADERS})
add_executable(${PROJECT_NAME}::unit-tests ALIAS unit-tests)
target_link_libraries(unit-tests PRIVATE ${PROJECT_NAME}::${PROJECT_NAME} Catch2::Catch2)
target_include_directories(unit-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# === Test ===
catch_discover_tests(unit-tests)
