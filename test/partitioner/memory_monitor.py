import subprocess
import psutil
import time
import matplotlib.pyplot as plt
from pathlib import Path


def monitor(command):
    # Start the C++ process
    proc = subprocess.Popen(command, shell=True)
    mem_usage = []
    mem_usage2 = []
    timestamps = []
    start_time = time.time()

    while proc.poll() is None: # While process is running
        try:
            # Get RSS memory in MB
            mem_info = psutil.Process(proc.pid).memory_full_info()
            mem = mem_info.uss / (1024 * 1024)
            mem_usage.append(mem)
        
            rss = mem_info.rss / (1024 * 1024)
            mem_usage2.append(rss)
            # rss = mem_info.rss / (1024 * 1024)   # Physical RAM
            # vms = mem_info.vms / (1024 * 1024)   # Virtual Memory
            timestamps.append(time.time() - start_time)
        except psutil.NoSuchProcess:
            break
        time.sleep(0.05) # Sample every 50ms

    return timestamps, mem_usage, mem_usage2

# Run and Plot
inputs = [
    "/workspaces/sb-graph/test/partitioner/external_tools/data/advection_1000.json",
    "/workspaces/sb-graph/test/partitioner/external_tools/data/advection_10000.json",
    "/workspaces/sb-graph/test/partitioner/external_tools/data/advection_100000.json"
]

colors = {
    inputs[0]: 'red',
    inputs[1]: 'blue',
    inputs[2]: 'green'
}

colors2 = {
    inputs[0]: 'brown',
    inputs[1]: 'skyblue',
    inputs[2]: 'black'
}


# Generate the plot
plt.figure(figsize=(10, 6))
plt.title("Memory Consumption Timeline")
plt.xlabel("Time (seconds)")
plt.ylabel("Memory Usage (MB)")
plt.grid(True)

for f in inputs:
    for _ in range(1):
        times, mem, mem2 = monitor(f"sleep 1; /workspaces/sb-graph/install/bin/sbg-partitioner -f {f} -p 16")

        # plt.plot(times, mem, color=colors[f])
        plt.plot(times, mem2, color=colors2[f], label=Path(f).stem)
plt.legend()

# Save the plot instead of showing it
output_file = "memory_benchmark_advection_16.svg"
plt.savefig(output_file, format='svg', bbox_inches='tight')
print(f"Benchmark finished. Results saved to {output_file}")