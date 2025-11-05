# sbg-parser-lib
add_library(sbg-parser-lib STATIC)

set_target_properties(sbg-parser-lib PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(sbg-parser-lib PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(
    sbg-parser-lib
    PRIVATE
    expr.cpp
    file_parser.cpp
    parser_exec.cpp
    sbg_program.cpp
    skipper.cpp
    statement.cpp
)

add_dependencies(sbg-parser-lib sbg-dev)

# Add executable
add_executable(
    sbg-parser
)

target_sources(
    sbg-parser
    PRIVATE
    main.cpp
)

find_package(Boost REQUIRED COMPONENTS program_options)

target_link_libraries(
    sbg-parser
    PRIVATE
    sbg-dev
    sbg-parser-lib
    sbg-util-lib
    Boost::program_options
)

# install
install(
    TARGETS sbg-parser sbg-parser-lib
    RUNTIME DESTINATION bin
    COMPONENT sbg-parser
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
    COMPONENT sbg-parser
)

install_lib_headers("${CMAKE_CURRENT_SOURCE_DIR}")
