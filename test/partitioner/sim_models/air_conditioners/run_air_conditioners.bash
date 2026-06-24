#!/bin/bash

echo "Starting to process partition files..."


# 1. Loop through all files ending in .partition
for file in *.partition; do
    
    # Check if any matching files actually exist (prevents errors if the folder is empty)
    [ -e "$file" ] || continue
    
    echo "--------------------------------------------"
    echo "Found partition file: $file"
    echo "--------------------------------------------"
    
    # 2. Run the simulation
    python3 ../sim_models.py -m airconds.mo -p "$file"

done

echo "All partition files processed!"