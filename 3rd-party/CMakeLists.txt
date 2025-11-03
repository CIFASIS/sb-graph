set(3RD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(GTEST_LIB "${CMAKE_CURRENT_SOURCE_DIR}/build/lib/libgtest.a")
set(GTEST_DIR "googletest-release-1.12.1")
set(GTEST_CODE  ${GTEST_DIR}.tar.gz)


add_custom_command(
    OUTPUT ${GTEST_LIB}  
    COMMAND ${CMAKE_COMMAND} -E echo "Building gtest."
    COMMAND ${CMAKE_COMMAND} -E tar xvzf ${3RD_PARTY_DIR}/gtest/${GTEST_CODE}
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${GTEST_DIR}/build
    COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/${GTEST_DIR}/build && cmake .. -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_DIR}/build
	COMMAND make -C ${CMAKE_CURRENT_BINARY_DIR}/${GTEST_DIR}/build install
)

add_custom_target(
    3rd-party-libs
    DEPENDS  ${GTEST_LIB}
    COMMAND install ${3RD_PARTY_DIR}/build/lib/lib* ${CMAKE_INSTALL_PREFIX}/lib
)

add_dependencies(sbgraph 3rd-party-libs)
