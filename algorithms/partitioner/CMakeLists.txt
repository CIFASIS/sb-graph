# sbg-part-library
add_library(sbg-part-lib STATIC)

set_target_properties(sbg-part-lib PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(sbg-part-lib PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(
    sbg-part-lib
    PRIVATE
        build_sb_graph.cpp
        dfs_on_sbg.cpp
        kernighan_lin_partitioner.cpp
        partition_graph.cpp
        partition_metrics_api.cpp
        partition_strategy.cpp
        weighted_sb_graph.cpp
)

add_dependencies(sbg-part-lib sbg-util-lib)


target_link_directories(
     sbg-part-lib
     PRIVATE
         sbg-util-lib
)

target_link_libraries(
    sbg-part-lib
    PRIVATE
        sbg-dev
        sbg-util-lib
)


# Add executable
add_executable(
    sbg-partitioner
)

target_sources(
    sbg-partitioner
    PRIVATE
        main.cpp
)

target_link_libraries(
    sbg-partitioner
    PRIVATE
        sbg-part-lib
        stdc++ 
        m
)

# # Add custom target for documentation
# add_custom_target(mmoc-doc
#   COMMAND doxygen ${PROJECT_SOURCE_DIR}/MMOCompiler.doxyfile
#   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#   COMMENT "Generating API documentation with Doxygen"
# )

# # Add test target
# add_custom_target(mmoc-test
#   COMMAND ${CMAKE_COMMAND} --build . --target mmoc
#   COMMAND ${CMAKE_COMMAND} -E chdir tests ${CMAKE_MAKE_PROGRAM} test
#   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#   COMMENT "Building and running tests"
# )


# install
install(
    TARGETS sbg-partitioner sbg-part-lib
    RUNTIME DESTINATION bin
    COMPONENT sbg-partitioner
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
    COMPONENT sbg-partitioner
)
