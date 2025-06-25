cmake_minimum_required(VERSION 3.16)

project(SBG)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build mode flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(SBG_PARTITIONER_LOGGING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall ") #-Werror  
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
endif()

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

add_custom_target(doc
      COMMAND echo "Generating Project documentation."
)

add_dependencies(doc eval-doc sbg-doc util-doc)

set_target_properties(doc PROPERTIES EXCLUDE_FROM_ALL ON)
