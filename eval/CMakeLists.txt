add_library(sbg-eval-lib OBJECT)

target_include_directories(sbg-eval-lib PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_sources(
    sbg-eval-lib
    PRIVATE
      eval_context.cpp
      eval_exec.cpp
      file_evaluator.cpp
      func_env.cpp
      input_translator.cpp
      pretty_print.cpp
      user_impl_map.cpp
      user_input.cpp
      var_env.cpp
)

add_subdirectory(visitors)

add_dependencies(sbg-eval-lib sbg-dev sbg-parser-lib)

add_executable(
    sbg-eval
    main.cpp
)

target_link_libraries(
    sbg-eval PRIVATE sbgraph
)
