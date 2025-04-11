# SBG library testing suite

This folder contains several files with the '.test' extension that can be used
as input of the ../bin binaries. They are also used as input of the parser and
eval unit tests. To run all tests use the `make test` command in the project
root directory. Currently three submodules are defined.

## performance

Measures and outputs execution time of predefined tests.
Currently sets and pws are profiled, as these are the base structs used in
SBGs algorithms. To add a new unit, define a `structname_perf.cpp` file in
the /test/performance folder, and add it to the PERF_SRC list in the Makefile.
In order to build /test/performance/bin/sbg-performance that executes all test
units call `make`.

Additionally, the subdmole /boost implements the construction of an ordinary
graph from a SBG to test the performance of traditional graph algorithms. To
use it execute /test/performance/bin/boost-perf. Alternatively script
/performance/boost/perf_boost.sh can be used, with the following arguments:
  a- A test file, i.e. the ones in the /test directory.
  b- A number representing the desired algorithm to execute, i.e.:
     0 Edmonds Matching, 1 Tarjan SCC, 2 Boost Topological Sort.
  c- A number to indicate how many runs of the test should be executed.
  d- A number that sets the value of N (size of the model) in the test.

## parser

Meant to check the correctness of the parser, executes system tests.
Expected results are placed in /test/parser/gt_data. To add a new unit include
its name in the parser_program list declared in /test/parser/parser_test.cpp
file. Then add the corresponding .log file in the /test/parser/gt_data folder.
The actual result of running each test will be present in each
/test/parser/test_data directory, with an additional file indicating if the
test succedeed or failed (in which case output differences between expected
and actual results).

## eval

Analogous to parser module, for the evaluator.

## Compilation and execution

Script /test/compile_run_tests.sh builds and runs all tests if no argument is
specified. It also accepts one argument to run an individual suite such as:
`performance`, `parser` or `eval`.  
