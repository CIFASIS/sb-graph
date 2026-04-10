# sbg-library
add_library(sbg-dev STATIC)

set_target_properties(sbg-dev PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(sbg-dev PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(
    sbg-dev
    PRIVATE
    bipartite_sbg.cpp
    directed_sbg.cpp
    dom_ord_pwmap.cpp
    expression.cpp
    fixed_points.cpp
    interval.cpp
    linear_expr.cpp
    map.cpp
    map_detail.cpp
    map_entry.cpp
    multidim_inter.cpp
    natural.cpp
    ord_pwmap.cpp
    ord_set.cpp
    ord_unidim_dense_set.cpp
    perimeter.cpp
    pw_map.cpp
    pwmap_detail.cpp
    pwmap_fact.cpp
    rational.cpp
    sbg.cpp
    set.cpp
    set_detail.cpp
    set_fact.cpp
    unord_pwmap.cpp
    unord_set.cpp
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
