# Add executable
add_executable(
    boost-perf
)

target_sources(
    boost-perf
    PRIVATE
        boost_exec.cpp
        main.cpp
        ordinary_graph.cpp    
        ordinary_graph_builder.cpp
)

target_compile_options(boost-perf PRIVATE "-Wall")

# Link the library
target_link_libraries(boost-perf
                      PRIVATE
                      sbg-dev
                      sbgraph
                      sbg-eval-lib
                      ${GOOGLE_TEST_LIB}
                      ${GOOGLE_MOCK_LIB}
                      pthread
)

target_include_directories(boost-perf PRIVATE ${GOOGLE_TEST_INCLUDE} ${PROJECT_SOURCE_DIR})

target_link_directories(boost-perf PRIVATE ${GOOGLE_TEST_INSTALL}/lib)

# Set output directory for executable
set_target_properties(boost-perf PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY
    ${BIN_DIR}
)
