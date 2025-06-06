#!/bin/bash
#===============================================================================
# 				 FILE: perf_boost.sh
#
# 				USAGE: perf_boost.sh FILE ALGORITHM ITERS SIZE
#
# 	DESCRIPTION: Helper script that given a SBG test file FILE replaces line
#                "N = ..." with "N = SIZE" and then finds the first
#                defined SBG in the file, constructing a traditional graph
#                with the BGL library. Then, executes the selected algorithm
#                according to option ALGORITHM. This is repeated ITERS times,
#                and the average of all executions is reported.
#
#    PARAMETERS: [FILE] Filename of the desired test.
#                [ALGORITHM] 0 = BGL Edmonds-Karp maximum matching, 1 = BGL
#                Tarjan SCC, 2 = BGL topological sort.
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
algo=$2
iterations=$3
size=$4

# Ignore multi-line comments
echo > aux_file
sed -r ':a;$!{N;ba};s|/\*[^*]*\*+([^/*][^*]*\*+)*/\n||' $file > aux_file

echo > test_file
echo > test_values
echo > builder_values

while read line; do
  if echo "$line" | grep -q "N = "; then 
    echo $line > eval; awk -v s=$size '{print "N = " s}' eval >> test_file; start=true;
  else
    echo $line >> test_file
  fi
done < aux_file


for i in $(seq "$iterations"); do
	echo $i
	./bin/boost-performance -a $algo -f test_file
	mv SBG.log SBG_${i}.log
	echo
    if [[ $algo==0 ]]; then
    while read line; do
      if  echo "$line" | grep -q "Boost Edmonds"; then echo $line > eval; awk '{print $7}' eval >> test_values; fi 
    done < "SBG_${i}.log"
    fi;

    if [[ $algo==1 ]]; then
    while read line; do
      if  echo "$line" | grep -q "Boost Tarjan"; then echo $line > eval; awk '{print $5}' eval >> test_values; fi 
      if  echo "$line" | grep -q "Scalar"; then echo $line > eval; awk '{print $6}' eval >> builder_values; fi 
    done < "SBG_${i}.log"
    fi;

    if [[ $algo==2 ]]; then
    while read line; do
      if  echo "$line" | grep -q "Boost Topological"; then echo $line > eval; awk '{print $5}' eval >> test_values; fi        
    done < "SBG_${i}.log"
    fi;
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
echo "Average algorithm:"
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
