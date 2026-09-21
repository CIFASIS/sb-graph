add_library(sbg-dev OBJECT)

target_include_directories(sbg-dev
    PRIVATE
      ${PROJECT_SOURCE_DIR}/include
      ${PROJECT_SOURCE_DIR}
)

target_sources(
    sbg-dev
    PRIVATE
    bipartite_sbg.cpp
    directed_sbg.cpp
    dom_ord_pwmap.cpp
    expression.cpp
    fixed_points.cpp
    integer.cpp
    interval.cpp
    linear_expr.cpp
    map.cpp
    map_detail.cpp
    map_entry.cpp
    multidim_inter.cpp
    ord_pwmap.cpp
    ord_set.cpp
    ord_unidim_dense_set.cpp
    perimeter.cpp
    pw_map.cpp
    pwmap_detail.cpp
    pwmap_impl.cpp
    rational.cpp
    sbg.cpp
    set.cpp
    set_detail.cpp
    set_impl.cpp
    unord_pwmap.cpp
    unord_set.cpp
)

add_custom_target(sbg-doc
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/doc/sbg
    COMMAND doxygen SBG.doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/sbg
    COMMENT "Generating API documentation with Doxygen"
)
