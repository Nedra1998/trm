# === Executable ===
file(GLOB_RECURSE EXAMPLES CONFIGURE_DEPENDS *.cpp)

foreach(file ${EXAMPLES})
  get_filename_component(file_name ${file} NAME_WE)
  add_executable(example-${file_name} ${file})
  target_link_libraries(example-${file_name} ${PROJECT_NAME}::${PROJECT_NAME})
  install(TARGETS example-${file_name} DESTINATION ${CMAKE_INSTALL_BINDIR})
endforeach()
