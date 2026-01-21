# Matching Benchmark
add_executable(match-benchmark)
target_sources(match-benchmark
  PRIVATE
  ../benchmark_main.cpp
  match_bm.cpp
  ../utils.cpp)
target_link_libraries(match-benchmark
  PRIVATE 
  benchmark::benchmark
  sbg-dev
  sbg-eval-lib)
set_target_properties(match-benchmark
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
add_custom_target(run-match
    COMMAND cd ${BIN_DIR} && ./match-benchmark
)
add_dependencies(run-match match-benchmark)

# Custom Matching Benchmark
add_executable(custom-match-benchmark)
target_sources(custom-match-benchmark
  PRIVATE
  custom_match_bm.cpp
  ../utils.cpp)
target_link_libraries(custom-match-benchmark
  PRIVATE
  benchmark::benchmark
  sbg-dev
  sbg-eval-lib)
set_target_properties(custom-match-benchmark
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
