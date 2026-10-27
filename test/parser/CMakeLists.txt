# Output directories
set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_data)

# Add executable
add_executable(
    parser-test
)

target_sources(
    parser-test
    PRIVATE
        parser_test.cpp
        main.cpp
)

target_compile_options(parser-test PRIVATE "-Wall")

# Link the library
target_link_libraries(parser-test
                      PRIVATE 
                      ${GOOGLE_TEST_LIB}
                      ${GOOGLE_MOCK_LIB}
                      pthread
)

target_include_directories(parser-test PRIVATE ${GOOGLE_TEST_INCLUDE})

target_link_directories(parser-test PRIVATE ${GOOGLE_TEST_INSTALL}/lib)

# Set output directory for executable
set_target_properties(parser-test PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY
    ${BIN_DIR}
)

add_custom_target(run-parser-test
      COMMAND echo "Running Eval tests."
      COMMAND rm -rf ${TEST_DATA_DIR}
      COMMAND mkdir -p ${TEST_DATA_DIR}
      COMMAND cd ${BIN_DIR} && ./parser-test
)

add_dependencies(run-parser-test parser-test)