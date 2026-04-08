# SBG library testing suite

This folder contains several files with the '.test' extension that can be used
as input of the  binaries. They are also used as input of the parser and
eval unit tests. Executing `make test` from sb-graph/build/ directory runs
all system tests for the parser, evaluator and partitioner.

## parser

Meant to check the correctness of the parser, executes system tests.
Expected results are placed in ./test/parser/gt_data. To add a new unit include
its name in the parser_program list declared in ./test/parser/parser_test.cpp
file. Then add the corresponding .log file in the ./test/parser/gt_data folder.
The actual result of running each test will be present in each
./test/parser/test_data directory, with an additional file indicating if the
test succedeed or failed (in which case output differences between expected
and actual results).

## eval

Analogous to parser module, for the evaluator.

## partitioner

Executes system tests for the partitioner.

## performance

Measures and outputs execution time of predefined tests. The corresponding
binary is sb-graph/test/build/bin/sbg-benchmark (to see available options run
the executable with --help as argument). For the algorithms benchmark it accepts
a single test file as a positional argument. The binary also supports Google
Benchmark options, i.e. --benchmark_filter=filter.

In the ./test/build/bin there are also some helpful binaries:
  - boost-perf: used to evaluate the performance of traditional graphs
    algorithms.
  - custom-boost-benchmark: benchmark for the C++ Boost Graph Library
    traditional scalar algorithms. This benchmark is introduced to compare
    the SBG approach with existing techniques. Environmental variable TEST_FILE
    is supported. The benchmark will start calculating some data, without
    printing results for some seconds. 
    If the process terminates with a Segmentation Fault, increase
    stack size with the following command: `ulimit -s stack_size` (128000 is
    recommended for stack_size).
