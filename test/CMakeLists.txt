set(GOOGLE_TEST_INSTALL ${PROJECT_SOURCE_DIR}/3rd-party/build)
set(GOOGLE_TEST_LIB gtest)
set(GOOGLE_MOCK_LIB gmock)
set(GOOGLE_TEST_INCLUDE ${GOOGLE_TEST_INSTALL}/include)


# Output directories
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(BIN_DIR ${BUILD_DIR}/bin)

# Create output directories
file(MAKE_DIRECTORY
      ${BUILD_DIR}
      ${BIN_DIR}
)

add_subdirectory(eval)
add_subdirectory(parser)
add_subdirectory(partitioner)
add_subdirectory(performance)

add_custom_target(test
      COMMAND echo "Running Tests."
)

add_dependencies(test run-eval-test run-parser-test run-partitioner-test run-performance-test)
