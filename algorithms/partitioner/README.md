# SBG Partitioner

The goal of this sub-project is to implement a load balancing algorithm to simulate discrete event systems in parallel using set based graphs.


## How to compile

`$ make all` or just `$make` compiles this collection of files, adding the implementation to the `sb-graph`. Also it outputs the `sbg-partitioner` binary. To run, it takes these arguments:

* `-f` path to the input file, a json file that represents the model we want to partitionate.
* `-p` number of partitions.
* `-g` [optional argument] output file path.
* `-o` [optional argument] output the sb graph.
* `-e` [optional argument] imbalance epsilon, a value between 0 and 1.

You can run `make MODE=Debug` to display debug messages. They will be useful to
understand how the graph is initially partitioned, and then how those partiions
are improved.

## How to run

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

`./bin/sbg-partitioner -f examples/air_conditioners.json -p 4 -g output.json`

The output will be written in the file `output.json` in JSON format:

```
{
    "partitions": [
        {"nodes": [
            [[0,24]],
            [[100,124]],
            [[200,224]],
            [[300,324]]
        ]},
        {"nodes": [
            [[25,49]],
            [[125,149]],
            [[225,249]],
            [[325,349]]
        ]},
        {"nodes": [
            [[50,74]],
            [[150,174]],
            [[250,274]],
            [[350,374]]
        ]},
        {"nodes": [
            [[75,99]],
            [[175,199]],
            [[275,299]],
            [[375,399]]
        ]}
    ]
}
```

where each `node` object is a set of intervals, that represents a set of nodes for each partition.


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
