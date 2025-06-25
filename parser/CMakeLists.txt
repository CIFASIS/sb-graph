# sbg-parser-lib
add_library(sbg-parser-lib STATIC)

set_target_properties(sbg-parser-lib PROPERTIES DEBUG_POSTFIX "d")
target_include_directories(sbg-parser-lib PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(
    sbg-parser-lib
    PRIVATE
        parser.cpp
        statement.cpp
        expr.cpp
        sbg_program.cpp
        skipper.cpp
)

add_dependencies(sbg-parser-lib sbg-dev)

target_link_libraries(
    sbg-eval-lib
    PRIVATE
        sbg-dev
)


# Add executable
add_executable(
    sbg-parser
)

target_sources(
    sbg-parser
    PRIVATE
        main.cpp
)

target_link_libraries(
    sbg-parser
    PRIVATE
        sbg-parser-lib
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
