
# Required CMake version
cmake_minimum_required(VERSION 3.5)

# Project setting
message("** Scanning System")

# Setup project name and programming language
project(nuclear C)
set(PROJECT_VERSION "0.1.0")

# Default build-type is 'Release'
set(Profile "Profile")
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

# Setup specific properties
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c11")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

# Setup 'Profile' build-type flags
set(CMAKE_C_FLAGS_PROFILE
    "${CMAKE_C_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage -pg"
    CACHE STRING "Flags used by the C compiler during profile builds."
    FORCE)
set(CMAKE_EXE_LINKER_FLAGS_PROFILE
    "" CACHE STRING
    "Flags used for linking binaries during profile builds."
    FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_PROFILE
    "" CACHE STRING
    "Flags used by the shared libraries linker during profile builds."
    FORCE)

mark_as_advanced(
    CMAKE_C_FLAGS_PROFILE
    CMAKE_EXE_LINKER_FLAGS_PROFILE
    CMAKE_SHARED_LINKER_FLAGS_PROFILE)

# Update the documentation string of CMAKE_BUILD_TYPE
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose type of build: Debug Release RelWithDebInfo MinSizeRel Profile."
    FORCE)

# Required CMake packages
find_package(PkgConfig REQUIRED)

# Setting up Glib 2.0
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.30)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})
add_definitions(${GLIB_CFLAGS_OTHER})

# Setting up Gtk+ 3.0
pkg_check_modules(GTK3 REQUIRED gtk+-3.0>=3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# Propagate to subdirectories
add_subdirectory(src)

# List found libraries
message("** External Libraries and Tools")
message("-- Glib found: ${GLIB_VERSION}")
message("-- Gtk+ found: ${GTK3_VERSION}")

# Display current configuration
message("** Configuration Summary")
message("-- Project name: ${CMAKE_PROJECT_NAME}")
message("-- Build type is: ${CMAKE_BUILD_TYPE}")
message("-- C Compiler: ${CMAKE_C_COMPILER}")
message("-- Source root directory: ${CMAKE_SOURCE_DIR}")
message("-- Build root directory: ${PROJECT_BINARY_DIR}")
