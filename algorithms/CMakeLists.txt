# Add subdirectories
add_subdirectory(cc)
add_subdirectory(matching)
add_subdirectory(mfvs)
add_subdirectory(misc)
#add_subdirectory(partitioner)
add_subdirectory(scc)
add_subdirectory(tearing)
add_subdirectory(sorting)

install_lib_headers("${CMAKE_CURRENT_SOURCE_DIR}")
