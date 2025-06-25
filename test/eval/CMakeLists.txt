# Output directories
set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_data)

# Create output directories
file(MAKE_DIRECTORY
      ${TEST_DATA_DIR}
)

# Add executable
add_executable(
    eval-test
)

target_sources(
    eval-test
    PRIVATE
        eval_test.cpp
        main.cpp
)

target_compile_options(eval-test PRIVATE "-Wall")

# Link the library
target_link_libraries(eval-test
                      PRIVATE 
                      ${GOOGLE_TEST_LIB}
                      ${GOOGLE_MOCK_LIB}
                      pthread
)

target_include_directories(eval-test PRIVATE ${GOOGLE_TEST_INCLUDE})

target_link_directories(eval-test PRIVATE ${GOOGLE_TEST_INSTALL}/lib)

# Set output directory for executable
set_target_properties(eval-test PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY
    ${BIN_DIR}
)

add_custom_target(run-eval-test
      COMMAND echo "Running Eval tests."
      COMMAND cd ${TEST_DATA_DIR} && rm -rf *
      COMMAND cd ${BIN_DIR} && ./eval-test
)

add_dependencies(run-eval-test eval-test)