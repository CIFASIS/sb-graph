# sbg-part-library
add_library(sbg-part-lib STATIC)

set_target_properties(sbg-part-lib PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(sbg-part-lib PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(
    sbg-part-lib
    PRIVATE
        build_sb_graph.cpp
        communication_cost.cpp
        dfs_on_sbg.cpp
        kernighan_lin_partitioner.cpp
        partition_graph.cpp
        partition_graph_cc.cpp
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
        pthread
)


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
