import glob
import os
import re
import subprocess
import sys

base_edges_dir = "../../.."
base_partition_dir = "../../../test/partitioner/external_tools/data"

# Pattern to capture model and size from filenames like "advection2D_100_edges.txt"
edge_pattern = re.compile(r"^(.*)_([0-9]+)_edges\.txt$")

for edges_path in glob.glob(os.path.join(base_edges_dir, "*_edges.txt")):
    filename = os.path.basename(edges_path)
    match = edge_pattern.match(filename)
    
    if not match:
        continue
        
    model, size = match.group(1), match.group(2)
    model_size = f"{model}_{size}"
    
    # Locate all partition files matching this model and size
    partition_glob = os.path.join(base_partition_dir, f"{model_size}.json-*.partition")
    partition_files = glob.glob(partition_glob)
    
    for partition_path in partition_files:
        cmd = ["python3", "compute_metrics.py", partition_path, edges_path]
        # print(f"Running: {' '.join(cmd)}")
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"[ERROR] Failed on {partition_path}:\n{result.stderr}", file=sys.stderr)
        else:
            print(result.stdout)