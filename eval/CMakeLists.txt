# sbg-part-library
add_library(sbg-eval-lib STATIC)

set_target_properties(sbg-eval-lib PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(sbg-eval-lib PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(
    sbg-eval-lib
    PRIVATE
      eval_context.cpp
      func_env.cpp
      impl_context.cpp
      pretty_print.cpp
      var_env.cpp
)

add_subdirectory(visitors)

add_dependencies(sbg-eval-lib sbg-dev sbg-parser-lib)

target_link_libraries(
    sbg-eval-lib
    PRIVATE
        sbgraph
        sbg-parser-lib
        sbg-util-lib
)


# Add executable
add_executable(
    sbg-eval
)

target_sources(
    sbg-eval
    PRIVATE
        main.cpp
)

target_link_libraries(
    sbg-eval
    PRIVATE
        sbg-eval-lib
)


add_custom_target(eval-doc
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/doc/eval
    COMMAND doxygen EVAL.doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/eval
    COMMENT "Generating API documentation with Doxygen"
)

set_target_properties(eval-doc PROPERTIES EXCLUDE_FROM_ALL ON)

# install
install(
    TARGETS sbg-eval sbg-eval-lib
    RUNTIME DESTINATION bin
    COMPONENT sbg-eval
    LIBRARY
        DESTINATION lib
        PERMISSIONS
            OWNER_READ
            OWNER_WRITE
            OWNER_EXECUTE
            GROUP_READ
            GROUP_EXECUTE
            WORLD_READ
            WORLD_EXECUTE
    COMPONENT sbg-eval
)

install_lib_headers("${CMAKE_CURRENT_SOURCE_DIR}")
