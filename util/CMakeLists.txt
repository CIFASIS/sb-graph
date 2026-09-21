add_library(sbg-util-lib STATIC)

target_include_directories(sbg-util-lib PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_sources(
    sbg-util-lib
    PRIVATE
    logger.cpp
    time_profiler.cpp
    user_input_handler.cpp
)

add_custom_target(util-doc
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/doc/util
    COMMAND doxygen UTIL.doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/util
    COMMENT "Generating API documentation with Doxygen"
)
