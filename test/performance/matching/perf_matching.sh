#!/bin/bash
#===============================================================================
# 				 FILE: perf_matching.sh
#
# 				USAGE: perf_matching.sh FILE ITERS SIZE
#
# 	DESCRIPTION: Helper script to execute multiple ITERS times the evaluator on
#                a matching test saved in FILE, with N set to SIZE.
#
#    PARAMETERS: [FILE] Filename of the desired test.
#                [ITERS] Number of executions of the test. The reported time
#                will be the average of all runs.
#                [SIZE] Sets the size of variable N in the test to SIZE.
#       OPTIONS: ---
#  REQUIREMENTS: [1] ../../../bin/sbg-eval binary. 
#         NOTES: --- 
#        AUTHOR: Denise Marzorati, denise.marzorati@gmail.com
#       PROJECT: Set Based Graph Library
#===============================================================================

file=$1
iterations=$2
size=$3

# Ignore multi-line comments
echo > aux_file
sed -r ':a;$!{N;ba};s|/\*[^*]*\*+([^/*][^*]*\*+)*/\n||' $file > aux_file

echo > test_values
echo > test_file

while read line; do
    if  echo "$line" | grep -q "N = "; then 
        echo $line > eval; awk -v s=$size '{print "N = " s}' eval >> test_file;
    else
        echo $line >> test_file
    fi
done < aux_file


for i in $(seq "$iterations"); do
	echo $i
	../../../bin/sbg-eval -f test_file
	mv SBG.log SBG_${i}.log
	echo
    while read line; do
        if  echo "$line" | grep -q "Total match"; then echo $line > eval; awk '{print $5}' eval >> test_values; fi        
    done < "SBG_${i}.log"
done

echo "Results:"

cat test_values

count=0;
total=0; 

for i in $( awk '{ print $1; }' test_values )
   do 
     total=$(echo $total+$i | bc )
     ((count++))
   done

echo 
echo "Average matching:"
echo "scale=2; $total / $count" | bc	
