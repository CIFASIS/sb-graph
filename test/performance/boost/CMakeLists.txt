# Custom Boost Benchmark
add_executable(custom-boost-benchmark)
target_sources(custom-boost-benchmark
  PRIVATE
  ../benchmark_main.cpp
  custom_boost_bm.cpp
  ordinary_graph.cpp
  ordinary_graph_builder.cpp
  ../utils.cpp)
target_link_libraries(custom-boost-benchmark
  PUBLIC
  benchmark::benchmark
  PRIVATE
  sbg-dev
  sbg-eval-lib)
set_target_properties(custom-boost-benchmark
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
