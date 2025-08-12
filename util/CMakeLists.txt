# sbg-part-library
add_library(sbg-util-lib STATIC)

set_target_properties(sbg-util-lib PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(sbg-util-lib PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(
    sbg-util-lib
    PRIVATE
    logger.cpp
    time_profiler.cpp
)

add_custom_target(util-doc
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/doc/util
    COMMAND doxygen UTIL.doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/util
    COMMENT "Generating API documentation with Doxygen"
)

set_target_properties(util-doc PROPERTIES EXCLUDE_FROM_ALL ON)

# install
install(
    TARGETS sbg-util-lib
    RUNTIME DESTINATION lib
)

install_lib_headers("${CMAKE_CURRENT_SOURCE_DIR}")
