include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Setup the main executable
set(SRC_LIST
  main.c
  ui/main_window.c
  reactor/reactor.c
)
add_executable(nuclear ${SRC_LIST})

# Link the main target
target_link_libraries(nuclear ${GLIB_LIBRARY_DIRS} ${GTK3_LIBRARIES})
