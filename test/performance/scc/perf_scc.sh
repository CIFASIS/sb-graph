#!/bin/bash

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
        if  echo "$line" | grep -q "Total SCC"; then echo $line > eval; awk '{print $5}' eval >> test_values; fi
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

algo=$algo+$total/$count;

echo 
echo "Average total:"
echo "scale=2; $algo" | bc	
