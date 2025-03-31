#!/bin/bash
set -e

if [[ -z "$1" || "$1" == "performance" ]]; then
  echo "BUILDING PERFORMANCE SYSTEM TESTS"
  (cd performance && make)
  echo "Done"

  echo "RUNNING PERFORMANCE SYSTEM TESTS"
  ./performance/bin/sbg-performance
  echo "Done"
fi

if [[ -z "$1" || "$1" == "parser" ]]; then
  SRC_FILES="../main.cpp ../parser_test.cpp"
  GTEST_INCLUDE="-I../../../3rd-party/gtest/usr/include"
  LD_FLAGS="-L../../../3rd-party/gtest/usr/lib -lgtest -lgmock -lpthread"
  COMPILE=("g++" "${GTEST_INCLUDE}" "-std=c++17" "${SRC_FILES}"
    "-o parser-test" "${LD_FLAGS}")

  echo "BUILDING PARSER SYSTEM TESTS"
  (cd parser && mkdir -p bin && rm -rf test_data && mkdir -p test_data)
  (cd parser/bin && ${COMPILE[@]})
  echo "Done"

  echo "RUNNING PARSER SYSTEM TESTS"
  (cd parser && ./bin/parser-test)
  echo "Done"
fi

if [[ -z "$1" || "$1" == "eval" ]]; then
  SRC_FILES="../main.cpp ../eval_test.cpp"
  GTEST_INCLUDE="-I../../../3rd-party/gtest/usr/include"
  LD_FLAGS="-L../../../3rd-party/gtest/usr/lib -lgtest -lgmock -lpthread"
  COMPILE=("g++" "${GTEST_INCLUDE}" "-std=c++17" "${SRC_FILES}" 
    "-o eval-test" "${LD_FLAGS}")

  echo "BUILDING EVALUATOR SYSTEM TESTS"
  (cd eval && mkdir -p bin && rm -rf test_data && mkdir -p test_data)
  (cd eval/bin && ${COMPILE[@]})
  echo "Done"

  echo "RUNNING EVALUATOR SYSTEM TESTS"
  (cd eval && ./bin/eval-test)
  echo "Done"
fi
