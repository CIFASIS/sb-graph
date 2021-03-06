#/bin/bash

MODEL=$1
GT_MODEL=./gt_data/${MODEL}/${MODEL}.c
TEST_MODEL_FOLDER=./test_data/${MODEL}
TEST_MODEL=${TEST_MODEL_FOLDER}/${MODEL}

mkdir ${TEST_MODEL_FOLDER}
TEST_FOLDER=`pwd`
echo ${TEST_FOLDER}
g++ ${GT_MODEL} -I../../../ -L../../../lib -o ${TEST_MODEL} -lsbgraph  
cd ${TEST_MODEL_FOLDER}
./${MODEL}
