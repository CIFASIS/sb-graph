target_sources(
    sbg-parser-lib
    PRIVATE
    expression.cpp
    sbg_program.cpp
    statement.cpp
)

install_lib_headers("${CMAKE_CURRENT_SOURCE_DIR}")
