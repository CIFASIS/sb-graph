# SBG Benchmark
add_executable(sbg-benchmark benchmark_main.cpp)

target_sources(sbg-benchmark
               PRIVATE
               benchmark_main.cpp
               dom_ord_pwmap_bm.cpp
               ord_set_bm.cpp)

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

# SCC Benchmark
add_executable(scc-benchmark benchmark_main.cpp scc/scc_bm.cpp)
target_link_libraries(scc-benchmark PUBLIC benchmark::benchmark PRIVATE sbg-dev sbg-eval-lib)
set_target_properties(scc-benchmark PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
add_custom_target(run-scc
    COMMAND cd ${BIN_DIR} && ./scc-benchmark
)
add_dependencies(run-scc scc-benchmark)

# SCC Benchmark
add_executable(custom-scc-benchmark benchmark_main.cpp scc/custom_scc_bm.cpp)
target_link_libraries(custom-scc-benchmark PUBLIC benchmark::benchmark PRIVATE sbg-dev sbg-eval-lib)
set_target_properties(custom-scc-benchmark PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
