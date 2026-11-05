#!/bin/bash

METHODS=("SBG")

call_partitioner_test() {
    local size="$1"
    echo Running garph partitioner for file $arg with $n parts
    for method in "${METHODS[@]}"; do
        file="advection2D_${size}.json"
        echo running -f $file -n $n -m $method
        "./install/bin/graph_partitioner" -f "$file" -n 4 -m "$method"
    done
}

# call_partitioner_test 100
call_partitioner_test 1000
call_partitioner_test 10000
