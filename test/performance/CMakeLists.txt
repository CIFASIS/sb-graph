# Add executable
add_executable(
    sbg-performance
)

target_sources(
    sbg-performance
    PRIVATE
        main.cpp
        pw_map_perf.cpp
        set_perf.cpp
)

target_compile_options(sbg-performance PRIVATE "-Wall")

# Link the library
target_link_libraries(sbg-performance
                      PRIVATE
                      sbg-dev
                      sbg-eval-lib
                      ${GOOGLE_TEST_LIB}
                      ${GOOGLE_MOCK_LIB}
                      pthread
)

target_include_directories(sbg-performance PRIVATE ${GOOGLE_TEST_INCLUDE} ${PROJECT_SOURCE_DIR})

target_link_directories(sbg-performance PRIVATE ${GOOGLE_TEST_INSTALL}/lib)

add_subdirectory(boost)

# Set output directory for executable
set_target_properties(sbg-performance PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY
    ${BIN_DIR}
)

add_custom_target(run-performance-test
      COMMAND echo "Running Performance Tests."
      COMMAND cd ${BIN_DIR} && ./sbg-performance
)

add_dependencies(run-performance-test sbg-performance)

# Benchmark executable
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
