# sbg-library
add_library(sbg-dev STATIC)

set_target_properties(sbg-dev PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(sbg-dev PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(
    sbg-dev
    PRIVATE
    af_map.cpp
    af_pwmap.cpp
    af_set.cpp
    interval.cpp
    lexp.cpp
    map.cpp
    multidim_inter.cpp
    multidim_lexp.cpp
    natural.cpp
    pw_map.cpp
    rational.cpp
    sbg.cpp
    set.cpp
)

add_custom_target(sbg-doc
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/doc/sbg
    COMMAND doxygen SBG.doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/sbg
    COMMENT "Generating API documentation with Doxygen"
)

# install
install(
    TARGETS sbg-dev
    RUNTIME DESTINATION lib
    COMPONENT sbg-dev
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
    COMPONENT sbg-dev
)

install_lib_headers("${CMAKE_CURRENT_SOURCE_DIR}")
