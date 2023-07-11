# SBG Library Testing Suite

In the /test folder several examples are presented, and they can be parsed or evaluated with the corresponding 
binary from the /bin directory. The `make test` command in the paren directory will evaluate all of the files 
and compare their results with the desired value stored in the /test/gt_data folder.

## For developers

To add a test unit to an existing test suite, a new directory should be create
in the gt_data folder. There a .cpp file should contain the testing code, and a
SBG.log archive is needed with the expected result of the test. The
result of applying the test will be saved in the test_data directory. If it is
correct there will be a "test.passed" file present.
