"""Write a partition printed by the partitioner as a parts file.

The input has one partition per line: its id, optionally followed by a comma, and
its set pieces, for example
    0, {[2:2]x[0:119]} {[3:9:111]x[0:39]}
The output has one line per vertex with the id of the partition it belongs to. A
vertex (v_0, ..., v_n) is written at line v_0 * size_1 * ... * size_n + ... + v_n,
the same layout the --dump-results flag uses.

Usage: python3 partition_to_parts.py <partition.txt> <parts.txt> [size_0 ... size_n]
If the sizes are not given, each one is the largest value in that dimension plus one.
"""

import itertools
import math
import re
import sys

PIECE = re.compile(r'\{([^}]*)\}')
INTERVAL = re.compile(r'\[(\d+):(\d+)(?::(\d+))?\]')


def parse_interval(match):
    begin, second, third = match.groups()
    if third is None:  # [begin:end]
        return range(int(begin), int(second) + 1)
    return range(int(begin), int(third) + 1, int(second))  # [begin:step:end]


def read_partitions(path):
    partitions = []
    for line in open(path):
        line = line.strip()
        if not line:
            continue
        part_id = int(re.match(r'\d+', line).group())
        pieces = [[parse_interval(m) for m in INTERVAL.finditer(p)] for p in PIECE.findall(line)]
        partitions.append((part_id, pieces))
    return partitions


def linear_index(vertex, sizes):
    index = 0
    for value, size in zip(vertex, sizes):
        index = index * size + value
    return index


def grid_sizes(partitions):
    """Size of each dimension: the largest value in it plus one."""
    dims = len(partitions[0][1][0])
    return tuple(max(piece[d][-1] for _, pieces in partitions for piece in pieces) + 1 for d in range(dims))


def build_parts(partitions, sizes):
    """Partition id of each vertex, in the linear order of the vertices."""
    parts = [-1] * math.prod(sizes)
    for part_id, pieces in partitions:
        for piece in pieces:
            for vertex in itertools.product(*piece):
                index = linear_index(vertex, sizes)
                if parts[index] != -1:
                    sys.exit(f'vertex {vertex} is in partitions {parts[index]} and {part_id}')
                parts[index] = part_id

    if -1 in parts:
        sys.exit(f'{parts.count(-1)} vertices have no partition, first one is at line {parts.index(-1)}')

    return parts


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)

    partitions = read_partitions(sys.argv[1])
    sizes = tuple(int(s) for s in sys.argv[3:]) or grid_sizes(partitions)
    parts = build_parts(partitions, sizes)

    with open(sys.argv[2], 'w') as output:
        output.writelines(f'{p}\n' for p in parts)
    print(f'wrote {math.prod(sizes)} vertices of a {"x".join(map(str, sizes))} grid in {len(partitions)} partitions to {sys.argv[2]}')


if __name__ == '__main__':
    main()
