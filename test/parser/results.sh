#/bin/bash
#===============================================================================
#
# 				 FILE: results.sh
#
# 				USAGE: results.sh FILE
#
# 	DESCRIPTION: Helper script to compare expected results of parser tests with
#                the execution of the current parsing version of the library.
#
#    PARAMETERS: [FILE] Name of the test (without extension). 
#       OPTIONS: ---
#  REQUIREMENTS: [1] A filename FILE.log in the ./gt_data (expected results).
#                [2] A filename FILE.test in the ./test_data directory (actual
#                    results).
#         NOTES: --- 
#        AUTHOR: Joaquin Fernandez and Denise Marzorati
#                , denise.marzorati@gmail.com
#       PROJECT: Set Based Graph Library
#===============================================================================

MODEL=$1
GT_MODEL="./gt_data/"${MODEL}
TEST_MODEL="./test_data/"${MODEL}

test_results () 
{
  if ! [ -f "$GT_FILE" ]; then
    echo "Model " $MODEL " test failed for " $TEST_FILE
    echo "File " $GT_FILE " don't exist." > $TEST_FILE.failed
  fi
  if ! [ -f "$TEST_FILE" ]; then
    echo "Model " $MODEL " test failed for " $TEST_FILE
    echo "File " $TEST_FILE " don't exist." > $TEST_FILE.failed
  fi
  RESULT="$(diff "$GT_FILE" "$TEST_FILE")"
  if [ -n "$RESULT" ]; then
    echo "Model " $MODEL " test failed for " $TEST_FILE
    diff "$GT_FILE" "$TEST_FILE" > $TEST_FILE.failed
  fi    
}

GT_FILE=${GT_MODEL}".log"
TEST_FILE=${TEST_MODEL}/${MODEL}".log"
test_results

# Check for error on the generated files.

TEST_FILE=${MODEL}".log"
[ -f "$TEST_MODEL/$TEST_FILE.failed" ] && exit

echo "Test passed" > ${TEST_MODEL}/$MODEL.passed
