# SCC Benchmark
add_executable(scc-benchmark)
target_sources(scc-benchmark
  PRIVATE
  ../benchmark_main.cpp
  scc_bm.cpp
  ../utils.cpp)
target_link_libraries(scc-benchmark
  PRIVATE
  benchmark::benchmark
  sbg-dev
  sbg-eval-lib)
set_target_properties(scc-benchmark
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
add_custom_target(run-scc
    COMMAND cd ${BIN_DIR} && ./scc-benchmark
)
add_dependencies(run-scc scc-benchmark)

# Custom SCC Benchmark
add_executable(custom-scc-benchmark)
target_sources(custom-scc-benchmark
  PRIVATE
  custom_scc_bm.cpp
  ../utils.cpp)
target_link_libraries(custom-scc-benchmark
  PRIVATE
  benchmark::benchmark
  sbg-dev
  sbg-eval-lib)
set_target_properties(custom-scc-benchmark
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
