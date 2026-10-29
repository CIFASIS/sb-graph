include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)

# ------------------------------------------------------------------------------
# GTest
# ------------------------------------------------------------------------------

find_package(GTest 1.12 QUIET)

if(NOT GTest_FOUND)
  message(STATUS "GTest not found in the system, downloading with FetchContent")

  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK ON CACHE BOOL "" FORCE)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
  )
  FetchContent_MakeAvailable(googletest)
endif()

# ------------------------------------------------------------------------------
# Google Benchmark
# ------------------------------------------------------------------------------

find_package(benchmark 1.6 QUIET)

if(NOT benchmark_FOUND)
  message(STATUS "GBenchmark not found in the system, downloading with FetchContent")

  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_TESTING     OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_INSTALL     OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    google_benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.6.1
  )
  FetchContent_MakeAvailable(google_benchmark)
endif()
