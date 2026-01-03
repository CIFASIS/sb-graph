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

Measures and outputs execution time of predefined tests.

The following benchmarks can be executed from the sb-graph/build/ directory
with the following commands:
  - `make run-benchmark`: benchmark for some implementations of Sets and PWMaps.
  - `make run-scc`: benchmark for the SCC algorithm applied to TestRL1.test,
    TestRL2.test and TestRL3.test.

In the ./test/build/bin there are also some helpful binaries:
  - boost-perf: used to evaluate the performance of traditional graphs
    algorithms.
  - custom-scc-benchmark: benchmark for the SCC algorithm applied to a custom
    .test file. Environmental variables TEST_FILE, SET_IMPL and PW_IMPL are
    supported. So, for example,
    `TEST_FILE="../../TestRL1.test" SET_IMP="2" ./custom-scc-benchmark` runs the
    benchmark for TestRL1.test with the ordered unidimensional dense set
    implementation.

There are also some scripts in some subfolders, that will be deleted in the
near future.
