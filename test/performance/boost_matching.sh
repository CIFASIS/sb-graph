#!/bin/bash

file=$1
iterations=$2
size=$3

echo > test_values
echo > test_file

while read line; do
    if  echo "$line" | grep -q "N = "; then 
        echo $line > eval; awk -v s=$size '{print "N = " s}' eval >> test_file;
    else
        echo $line >> test_file
    fi
done < $file


for i in $(seq "$iterations"); do
	echo $i
	./boost/bin/boost-performance -f test_file
	mv SBG.log SBG_${i}.log
	echo
    while read line; do
        if  echo "$line" | grep -q "Boost"; then echo $line > eval; awk '{print $7}' eval >> test_values; fi        
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
echo "Average:"
echo "scale=2; $total / $count" | bc	


