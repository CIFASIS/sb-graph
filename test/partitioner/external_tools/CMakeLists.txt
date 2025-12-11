cmake_minimum_required(VERSION 3.16)

project(ExtPartitioners)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add libraries
add_library(graph-partitioner STATIC)

set_target_properties(graph-partitioner PROPERTIES DEBUG_POSTFIX "d")

target_include_directories(graph-partitioner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(graph-partitioner PRIVATE 3rd-party/KaHIP/deploy)

target_sources(
    graph-partitioner
    PRIVATE
        graph_partitioner.cpp
)


# Source files.
add_subdirectory(3rd-party)

# Add executable
add_executable(
    graph_partitioner
)

target_sources(
    graph_partitioner
    PRIVATE
        main.cpp
)

target_link_directories(graph_partitioner PRIVATE 3rd-party/KaHIP/deploy)

target_link_libraries(
    graph_partitioner
    PRIVATE
        graph-partitioner
        metis
        scotch
        scotcherr
        kahip.a   
)

# install
install(
    TARGETS graph_partitioner
    RUNTIME DESTINATION bin
    COMPONENT graph_partitioner
)