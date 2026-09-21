add_library(sbg-parser-lib OBJECT)

target_include_directories(sbg-parser-lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_sources(
    sbg-parser-lib
    PRIVATE
    expr.cpp
    file_parser.cpp
    parser_exec.cpp
    sbg_program.cpp
    skipper.cpp
    statement.cpp
    ${PROJECT_SOURCE_DIR}/ast/expression.cpp
    ${PROJECT_SOURCE_DIR}/ast/sbg_program.cpp
    ${PROJECT_SOURCE_DIR}/ast/statement.cpp
)

add_executable(
    sbg-parser
    main.cpp
)

target_link_libraries(sbg-parser PRIVATE sbgraph)
