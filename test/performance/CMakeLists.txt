# SBG Benchmark for different Set and PWMap implementations
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

add_subdirectory(boost)
add_subdirectory(matching)
add_subdirectory(scc)
