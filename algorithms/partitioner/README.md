# SBG Partitioner

The goal of this sub-project is to implement a load balancing algorithm to simulate discrete event systems in parallel using set based graphs.

## How to run it

The input file must be a json file with the following format:


```
Var: {
    id : string
    cost : int
    exp : (a, b) where a, b ∈ Nat and a represents the coefficient and b the constant
    of the expression a × i + b
    def : {n1 , . . . , nk } set of node ids
}

Node: {
    id: string
    weight: int
    interval: [1, . . . , M ]
    lhs: {L1 , . . . , Lk } where Li has type Var
    rhs : {R1 , . . . , Rg } where Ri hast type Var
}
```

For example:

```
{
    "nodes": [
        {
            "id": 1,
            "interval": [[1, 100]],
            "lhs": [
                {
                    "id": "th",
                    "exp": [[1, 0]],
                    "defs": []
                }
            ],
        },
    .
    .
    .
    ]
}
```

To run an example and get the resultant partition you can run:

`./bin/sbg-partitioner -f examples/air_conditioners.json -p 4`


## Test suites

Test cases were implemented in [test/partitioner/](/test/partitioner/) folder, which intends to test different modules of code (like the creationg of computational graphs, initial partitions, among others).

To execute them you have to run `$make test`. You have to run `make` since the test suite uses `sb-graph` library


## Third Party Libraries

In the implementation of this project we used several third party libraries,
all of them are open source.

* [Set Based Graph Library](https://github.com/CIFASIS/sb-graph) to use set based graphs
in the branch `sb-graph-dev`.
* [boost](https://www.boost.org/), version 1.8.4, mainly as a sbg library dependency.
* [RapidJSON](https://rapidjson.org/) to read and write JSON files.
* [Google Test](https://google.github.io/googletest/) for test cases.

## How to get Metrics

### Metrics

`$make sbg-partitioner-metrics` compiles and outputs sbg-partitioner-metrics binary, which partitionates and outputs edge cut, communication volume, maximum communication volume and maximum imbalance of the input and of the files in the directory passed as an argument. To run, it takes these arguments:

* `-f` path to the input file, a json file that represents the model we want to partitionate.
* `-d` path to a directory with txt files that indicate the model partitioning using other algorithms.
* `-p` number of partitions.
* `-e` [optional argument] imbalance epsilon, a value between 0 and 1.

Output files with the metrics will be output in the directory passed as an argument.


## External tools

# Graph Partitioner (`grap_partitioner`)

A command-line tool designed to partition graphs using various partitioning algorithms and balance constraints. It provides execution and computation time metrics to help benchmark and analyze partitioning performance across different algorithms.

---

## Usage

```bash
external_tools/install/bin/graph_partitioner -f <file_name> -n <num_partitions> -m <partition_method> -i <imbalance>

```

---

## Command-Line Options

| Flag | Argument | Description | Required |
| --- | --- | --- | --- |
| `-f` | `<file_name>` | Path to the SBG input JSON file or generated graph input. | Yes |
| `-n` | `<num_partitions>` | Target number of partitions (integer). | Yes |
| `-m` | `<partition_method>` | Partitioning algorithm to execute. | Yes |
| `-i` | `<imbalance>` | Allowed partition imbalance ratio/threshold. | Yes |
| `-h` | — | Display usage information and exit. | No |

---

## Supported Partition Methods (`-m`)

* **`SBG`** — Streaming/Balanced Graph partitioner
* **`Scotch`** — Dual/Static graph partitioner
* **`Metis`** — Family of serial partitioning algorithms

---

## Examples

Partition a graph file into 4 parts using Metis with an imbalance factor of 0.03:

```bash
./graph_partitioner -f input_graph.json -n 4 -m Metis -i 0.03

```

Run partitioning using KaHIP on a generated graph:

```bash
./graph_partitioner -f data/network.sbg.json -n 8 -m SBG -i 0.05

```

Display help:

```bash
./graph_partitioner -h

```
