# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

# Project-level configuration.
set(PROJECT_NAME "gokai")
project(${PROJECT_NAME} LANGUAGES CXX C)

# This value is used when generating builds using this plugin, so it must
# not be changed.
set(PLUGIN_NAME "gokai_plugin")

cmake_policy(SET CMP0079 NEW)
cmake_policy(SET CMP0140 NEW)

function(ADD_PLUGIN_SYM TARGET)
  if(IS_SYMLINK "${TARGET}")
    file(READ_SYMLINK ${TARGET} TARGET_REAL)

    if(NOT IS_ABSOLUTE "${TARGET_REAL}")
      get_filename_component(TARGET_DIR "${TARGET}" DIRECTORY)
      set(TARGET_REAL "${TARGET_DIR}/${TARGET_REAL}")
    endif()

    add_plugin_sym("${TARGET_REAL}")
  endif()

  set(GOKAI_LIBRARIES "${GOKAI_LIBRARIES}" "${TARGET}")
  return(PROPAGATE GOKAI_LIBRARIES)
endfunction()

pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk+-3.0 gtk+-wayland-3.0)
pkg_check_modules(WAYLAND REQUIRED IMPORTED_TARGET wayland-client)
pkg_check_modules(GOKAI_API REQUIRED IMPORTED_TARGET gokai-core gokai-api)

# Any new source files that you add to the plugin should be added here.
list(APPEND PLUGIN_SOURCES
  "gokai_plugin.cc"
  "src/application.cc"
)

# Define the plugin library target. Its name must not be changed (see comment
# on PLUGIN_NAME above).
add_library(${PLUGIN_NAME} SHARED
  ${PLUGIN_SOURCES}
)

# Apply a standard set of build settings that are configured in the
# application-level CMakeLists.txt. This can be removed for plugins that want
# full control over build settings.
apply_standard_settings(${PLUGIN_NAME})

# Symbols are hidden by default to reduce the chance of accidental conflicts
# between plugins. This should not be removed; any symbols that should be
# exported should be explicitly exported with the FLUTTER_PLUGIN_EXPORT macro.
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)

# Source include directories and library dependencies. Add any plugin-specific
# dependencies here.
message("${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PLUGIN_NAME} PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PLUGIN_NAME} PRIVATE flutter)
target_link_libraries(${PLUGIN_NAME} PRIVATE PkgConfig::GTK)
target_link_libraries(${PLUGIN_NAME} PRIVATE PkgConfig::WAYLAND)
target_link_libraries(${PLUGIN_NAME} PRIVATE PkgConfig::GOKAI_API)

add_subdirectory(protocols)
target_link_libraries(${BINARY_NAME} PRIVATE gokai-wayland-protocols)

# List of absolute paths to libraries that should be bundled with the plugin.
# This list could contain prebuilt libraries, or libraries created by an
# external build triggered from this build file.

set(GOKAI_LINK_LIBRARIES ${GTK_LINK_LIBRARIES} ${WAYLAND_LINK_LIBRARIES} ${GOKAI_API_LINK_LIBRARIES})
list(REMOVE_ITEM GOKAI_LINK_LIBRARIES z)
foreach(LIB IN LISTS GOKAI_LINK_LIBRARIES)
  add_plugin_sym("${LIB}")
endforeach()

set(gokai_bundled_libraries ${GOKAI_LIBRARIES} PARENT_SCOPE)

# === Tests ===
# These unit tests can be run from a terminal after building the example.

# Only enable test builds when building the example (which sets this variable)
# so that plugin clients aren't building the tests.
if (${include_${PROJECT_NAME}_tests})
if(${CMAKE_VERSION} VERSION_LESS "3.11.0")
message("Unit tests require CMake 3.11.0 or later")
else()
set(TEST_RUNNER "${PROJECT_NAME}_test")
enable_testing()

# Add the Google Test dependency.
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.11.0.zip
)
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Disable install commands for gtest so it doesn't end up in the bundle.
set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of googletest" FORCE)

FetchContent_MakeAvailable(googletest)

# The plugin's exported API is not very useful for unit testing, so build the
# sources directly into the test binary rather than using the shared library.
add_executable(${TEST_RUNNER}
  test/gokai_plugin_test.cc
  ${PLUGIN_SOURCES}
)
apply_standard_settings(${TEST_RUNNER})
target_include_directories(${TEST_RUNNER} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${TEST_RUNNER} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${TEST_RUNNER} PRIVATE flutter)
target_link_libraries(${TEST_RUNNER} PRIVATE PkgConfig::GTK)
target_link_libraries(${TEST_RUNNER} PRIVATE PkgConfig::GOKAI_API)
target_link_libraries(${TEST_RUNNER} PRIVATE gtest_main gmock)

# Enable automatic test discovery.
include(GoogleTest)
gtest_discover_tests(${TEST_RUNNER})

endif()  # CMake version check
endif()  # include_${PROJECT_NAME}_tests
