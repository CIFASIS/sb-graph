# Add subdirectories
add_subdirectory(cc)
add_subdirectory(cutvertex)
add_subdirectory(matching)
add_subdirectory(misc)
add_subdirectory(partitioner)
add_subdirectory(scc)
add_subdirectory(structure)
add_subdirectory(toposort)

install_lib_headers("${CMAKE_CURRENT_SOURCE_DIR}")
