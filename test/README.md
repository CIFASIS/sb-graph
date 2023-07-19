# SBG library testing suite

This folder contains several files with the '.test' extension that can be used
as input of the ../bin binaries. They are also used as input of the parser and
eval unit tests. To run all tests use the `make test` command in the parent
directory.

## For developers

Currently thre submodules are defined:

  * performance 

  * parser

  * eval

The first one only measures and outputs execution time of user defined tests.
To profile a new struct a `structname_perf.cpp` file should be created where all
tests could be defined.

The last two check the soundness of the parser and the evaluator with already
defined tests, specifying the input and the expected result. To test a new
struct create a `structname.test` file in the current directory, and create
parser/gt_data/structname and eval/gt_data/structname folders. In each of them
there should be a structname.cpp file with code that logs its result to the
SBG::Util::SBG_LOG structure; and a SBG.log archive with the expected result of
the execution of the .cpp file. Then add structname to 'parser_program' and
'eval_program' constants in the respective parser/parser_test.cpp and
eval/eval_test.cpp files. The actual value of the running each test will be
present in the parser/test_data and eval/test_data folders.
