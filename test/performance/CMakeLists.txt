# SBG Benchmark for different Set, PWMap, and SBG algorithms implementations
add_executable(sbg-benchmark main.cpp)

target_sources(sbg-benchmark
               PRIVATE
               boost/scalar_graph.cpp
               boost/scalar_graph_builder.cpp
               boost/scc_graph_builder.cpp
               main.cpp
               bm_exec.cpp
               matching_bm.cpp
               pwmap_bm.cpp
               scc_bm.cpp
               set_bm.cpp
               utils.cpp)

set_target_properties(sbg-benchmark PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY
    ${BIN_DIR}
)

add_custom_target(run-benchmark
      COMMAND echo "Running SBG Benchmark."
      COMMAND cd ${BIN_DIR} && ./sbg-benchmark
)

target_link_libraries(sbg-benchmark
                      PUBLIC
                      benchmark::benchmark
                      PRIVATE
                      sbg-dev
                      sbg-eval-lib)
