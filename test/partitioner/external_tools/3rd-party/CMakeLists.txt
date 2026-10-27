set(3RD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(KAHIP_LIB "${CMAKE_INSTALL_PREFIX}/lib/libkahip.a")
set(SBG_LIB "${CMAKE_INSTALL_PREFIX}/lib/libsbgraph.a")

option(BUILD_KAHIP_LIB "Build KaHIP Library" OFF)

if (BUILD_KAHIP_LIB)
    add_custom_command(
        OUTPUT ${KAHIP_LIB}
        COMMAND ${CMAKE_COMMAND} -E echo "Building KaHIP."
        COMMAND ${CMAKE_COMMAND} -E tar xvzf ${3RD_PARTY_DIR}/KaHIP/KaHIP-3.16.tar.gz
        COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/KaHIP-3.16 && ./compile_withcmake.sh 

        COMMAND ${CMAKE_COMMAND} -E echo "Installing KaHIP."
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${3RD_PARTY_DIR}/KaHIP/deploy
        COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/KaHIP-3.16/deploy ${3RD_PARTY_DIR}/KaHIP/
    )
else()
    add_custom_command(
        OUTPUT ${KAHIP_LIB}
        COMMAND ${CMAKE_COMMAND} -E echo "Skipping KaHIP installation."
    )

endif()

add_custom_command(
    OUTPUT ${SBG_LIB}
    COMMAND ${CMAKE_COMMAND} -E echo "Installing SBG Library. ${3RD_PARTY_DIR}/sbg"
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${3RD_PARTY_DIR}/sbg
    COMMAND mkdir -p ${3RD_PARTY_DIR}/sbg
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../install/lib/* ${3RD_PARTY_DIR}/sbg
)


add_custom_target(
    3rd-party-libs
    DEPENDS  ${KAHIP_LIB}
    DEPENDS ${SBG_LIB}
)

add_dependencies(graph-partitioner 3rd-party-libs)
