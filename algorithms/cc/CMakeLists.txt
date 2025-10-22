# sbg-cc-library
add_library(sbg-cc-lib STATIC)

target_include_directories(sbg-cc-lib PUBLIC ${PROJECT_SOURCE_DIR})

target_sources(
    sbgraph
    PRIVATE
    cc.cpp
)


target_sources(
    sbg-cc-lib
    PRIVATE
    cc.cpp
)
