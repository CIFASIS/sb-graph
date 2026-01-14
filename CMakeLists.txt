cmake_minimum_required(VERSION 3.16)

project(SBG)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build mode flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(SBG_PARTITIONER_LOGGING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall ") # -Werror
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder -fconcepts -O3")
endif()

# Output directories
set(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)

# Create output directories
file(MAKE_DIRECTORY
  ${BIN_DIR}
  ${LIB_DIR}
)

# Define install rules for library headers.
function(install_lib_headers headers_dir)
  file(GLOB_RECURSE PROJECT_HEADERS
    "${headers_dir}/*.hpp"
  )

  get_filename_component(CURRENT_DIR_BASENAME "${headers_dir}" NAME)

  install(FILES
    ${PROJECT_HEADERS}
    DESTINATION "include/${CURRENT_DIR_BASENAME}"
  )
endfunction()

# sbgraph-library
add_library(sbgraph STATIC)

# Add subdirectories
add_subdirectory(3rd-party)
add_subdirectory(algorithms)
add_subdirectory(eval)
add_subdirectory(parser)
add_subdirectory(ast)
add_subdirectory(sbg)
add_subdirectory(test)
add_subdirectory(util)

set_target_properties(sbgraph PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(sbgraph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sbgraph sbg-dev)
add_dependencies(sbgraph sbg-dev)

# install
install(
  TARGETS sbgraph
  RUNTIME DESTINATION lib
  COMPONENT sbgraph
  LIBRARY
  DESTINATION lib
  PERMISSIONS
  OWNER_READ
  OWNER_WRITE
  OWNER_EXECUTE
  GROUP_READ
  GROUP_EXECUTE
  WORLD_READ
  WORLD_EXECUTE
  COMPONENT sbgraph
)

# Documentation
set(DOC_PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
string(REPLACE ";" " " DOXYGEN_INPUT_DIR "${DOC_PATHS}")

set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/doc")

set(DOXYGEN_EXCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(DOXYGEN_MAINPAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
               ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

add_custom_target(doc
  COMMAND echo "Generating Project documentation."
  COMMAND doxygen "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
  #COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/doc
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating API documentation with Doxygen"
)

set_target_properties(doc PROPERTIES EXCLUDE_FROM_ALL ON)
