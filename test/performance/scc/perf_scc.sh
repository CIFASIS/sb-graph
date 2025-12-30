#!/bin/bash
#===============================================================================
# 				 FILE: perf_scc.sh
#
# 				USAGE: perf_scc.sh FILE ITERS SIZE
#
# 	DESCRIPTION: Helper script to execute ITERS times the evaluator on
#                a SCC test saved in FILE, with N set to SIZE.
#
#    PARAMETERS: [FILE] Filename of the desired test.
#                [ITERS] Number of executions of the test. The reported time
#                will be the average of all runs.
#                [SIZE] Sets the size of variable N in the test to SIZE.
#       OPTIONS: ---
#  REQUIREMENTS: [1] ../../../build/eval/sbg-eval binary. 
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

echo > test_file
echo > test_values
echo > test_builder
echo > builder_values

while read line; do
    if  echo "$line" | grep -q "N = "; then 
        echo $line > eval; awk -v s=$size '{print "N = " s ";"}' eval >> test_file;
    else
        echo $line >> test_file
    fi
done < aux_file

for i in $(seq "$iterations"); do
	echo $i
	../../../build/eval/sbg-eval test_file -p 1
	mv SBG.log SBG_${i}.log
	echo
    while read line; do
        if  echo "$line" | grep -q "Total MinReachSCC"; then echo $line > eval; awk '{print $5}' eval >> test_values; fi
        if  echo "$line" | grep -q "SBG SCC builder"; then echo $line > eval; awk '{print $4}' eval >> builder_values; fi
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
echo "Average SCC:"
echo "scale=2; $total / $count" | bc	

algo=$total/$count;
count=0;
total=0; 

for i in $( awk '{ print $1; }' builder_values )
   do 
     total=$(echo $total+$i | bc )
     ((count++))
   done

echo 
echo "Average builder:"
echo "scale=2; $total / $count" | bc	

total=$algo+$total/$count;

echo 
echo "Average total:"
echo "scale=2; $total" | bc	
