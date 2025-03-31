#/bin/bash

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
