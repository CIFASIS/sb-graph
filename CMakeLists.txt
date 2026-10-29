cmake_minimum_required(VERSION 3.16)

# ------------------------------------------------------------------------------
# Project configuration
# ------------------------------------------------------------------------------

project(
  SBGraph
  VERSION 5.0.0
  LANGUAGES CXX
)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(
    CMAKE_INSTALL_PREFIX
    "$ENV{HOME}/.local/sbgraph"
    CACHE PATH
    "Installation prefix"
    FORCE
  )
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# Build modes: Debug, Release
# ------------------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ------------------------------------------------------------------------------
# Compilation flags
# ------------------------------------------------------------------------------

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(SBG_PARTITIONER_LOGGING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall ") # -Werror
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder -fconcepts -O3")
endif()

# ------------------------------------------------------------------------------
# Output directories
# ------------------------------------------------------------------------------

set(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)

file(MAKE_DIRECTORY
  ${BIN_DIR}
  ${LIB_DIR}
)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

find_package(Boost REQUIRED COMPONENTS program_options)

# ------------------------------------------------------------------------------
# SBGraph library
# ------------------------------------------------------------------------------

add_library(sbgraph STATIC)
add_library(SBG::sbgraph ALIAS sbgraph)

target_compile_features(sbgraph PUBLIC cxx_std_17)

target_include_directories(sbgraph
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)

target_link_libraries(sbgraph
  PUBLIC
    Boost::program_options
)

set_target_properties(sbgraph PROPERTIES
  EXPORT_NAME sbgraph
  OUTPUT_NAME sbgraph
)

# ------------------------------------------------------------------------------
# Internal components of the library
# ------------------------------------------------------------------------------

add_subdirectory(util)
add_subdirectory(sbg)
add_subdirectory(parser)
add_subdirectory(eval)
add_subdirectory(algorithms)

option(SBG_BUILD_TESTS "Build SBGraph tests" ON)
if(SBG_BUILD_TESTS)
  add_subdirectory(3rd-party)
  add_subdirectory(test)
endif()

target_sources(sbgraph
  PRIVATE
    # Objetos de los componentes internos.
    $<TARGET_OBJECTS:sbg-dev>
    $<TARGET_OBJECTS:sbg-util-lib>
    $<TARGET_OBJECTS:sbg-parser-lib>
    $<TARGET_OBJECTS:sbg-eval-lib>
)

# ------------------------------------------------------------------------------
# Library installation
# ------------------------------------------------------------------------------

install(
  TARGETS sbgraph
  EXPORT SBGraphTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY
    include/sbgraph
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
    PATTERN "*.hpp"
)

install(
  EXPORT SBGraphTargets
  FILE SBGraphTargets.cmake
  NAMESPACE SBG::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SBGraph
)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SBGraphConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/SBGraphConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SBGraph
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/SBGraphConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SBGraphConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SBGraphConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SBGraph
)

# ------------------------------------------------------------------------------
# Documentation
# ------------------------------------------------------------------------------

option(SBG_BUILD_DOCS "Build SBGraph documentation" OFF)

set(DOC_PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
string(REPLACE ";" " " DOXYGEN_INPUT_DIR "${DOC_PATHS}")

set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/doc")

set(DOXYGEN_EXCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(DOXYGEN_MAINPAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
               ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

if(SBG_BUILD_DOCS)
  add_custom_target(doc
    COMMAND echo "Generating Project documentation."
    COMMAND doxygen "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/doc
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
  )

  set_target_properties(doc PROPERTIES EXCLUDE_FROM_ALL ON)
endif()
