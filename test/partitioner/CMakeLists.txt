# Add executable
add_executable(
    partitioner-test
)

target_sources(
    partitioner-test
    PRIVATE
        main.cpp
        partitioner_test.cpp
)

target_compile_options(partitioner-test PRIVATE "-Wall")

# Link the library
target_link_libraries(partitioner-test
    PRIVATE
    sbg-part-lib
    GTest::gtest
    GTest::gmock
    GTest::gtest_main
    pthread
)

target_include_directories(partitioner-test PRIVATE ${PROJECT_SOURCE_DIR})

# Set output directory for executable
set_target_properties(partitioner-test PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY
    ${BIN_DIR}
)

add_custom_target(run-partitioner-test
      COMMAND echo "Running Partitioner Tests."
      COMMAND cd ${BIN_DIR} && ./partitioner-test
)

add_dependencies(run-partitioner-test partitioner-test)
