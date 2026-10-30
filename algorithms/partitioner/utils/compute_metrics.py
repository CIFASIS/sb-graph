import numpy as np
import pandas as pd
import sys
import os

def compute_partition_metrics(partition_path, graph_path):
    # 1. Efficiently load the data
    print(os.path.basename(partition_path), end=' ')
    # Adjust sep if your file uses something other than whitespace
    edges = np.loadtxt(graph_path, dtype=np.int32)
    partitions = np.loadtxt(partition_path, dtype=np.int32)

    src = edges[:, 0]
    tgt = edges[:, 1]

    # 2. Identify Cut Edges
    # node_partitions[node_id] gives the partition ID
    p_src = partitions[src]
    p_tgt = partitions[tgt]
    
    cut_mask = p_src != p_tgt
    edge_cut = np.sum(cut_mask)

    # 3. Calculate Communication Volume per Partition
    # We need unique pairs of (source_partition, target_partition) 
    # for every node that has an external neighbor.
    
    # Filter only the cut edges
    cut_src_nodes = src[cut_mask]
    cut_tgt_partitions = p_tgt[cut_mask]
    cut_src_partitions = p_src[cut_mask]

    # Create a unique key for (SourceNode, TargetPartition)
    # This represents: "Node X needs to talk to Partition Y"
    node_to_part_key = (cut_src_nodes.astype(np.int64) << 32) | cut_tgt_partitions.astype(np.int64)
    
    # Get unique interactions only
    unique_keys = np.unique(node_to_part_key)
    
    # Extract the source node ID back from the unique keys to find its partition
    # (Bit-shifting back down to get the original 32-bit source node ID)
    unique_src_nodes = (unique_keys >> 32).astype(np.int32)
    unique_src_partitions = partitions[unique_src_nodes]

    # 4. Aggregate by Partition
    # Count how many unique "node-to-remote-partition" interactions each partition has
    part_series = pd.Series(unique_src_partitions)
    volume_per_partition = part_series.value_counts()

    total_comm_volume = volume_per_partition.sum()
    max_comm_volume = volume_per_partition.max()
    max_part_id = volume_per_partition.idxmax() if volume_per_partition.size > 0 else 0

    return {
        "edge_cut": str(edge_cut),
        "total_volume": str(total_comm_volume),
        "max_volume": str(max_comm_volume),
        "max_partition": str(max_part_id)
    }

results = compute_partition_metrics(sys.argv[1], sys.argv[2])
print(results)