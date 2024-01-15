**Matching Algorithm test models**

This folder contains four SBG test models, where the program constructs the corresponding SB-Graph, and then applies the SBG matching algorithm so that they can be compared with the matching algorithm from the BGL library.

The models are the following: 

* A simple model with small algebraic loops disconnected from each other `Test1.test` and `Test1.mo`. 
* A model with connections between different indices `Test2.test`  and `Test2.mo`.
* Similar to Test2, but with border conditions changed `Test3.test` and `Test3.mo`.
* The last one representing two copies of Test3 called `Test4.test` and `Test4.mo`.

In order to run the matching algorithm the following steps must be taken:

* Build and install the SBGraph library as described [here](https://github.com/CIFASIS/sb-graph#installation)
* From the sb-graph root folder run: `./bin/sbg-eval -f ./test/matching/{TEST_MODEL.test}` where `{TEST_MODEL.test}` is one of the models defined above.

**Note:** 
The execution will generate a SBG.log file located in the same folder where the command is called, this file contains detailed information about the different steps taken by the matching algorithm
and also information about the generated graph used by the algorithm.

Additionally, there's a performance application called `boost-cmp` that  applies the boost Edmonds Maximum Cardinality matching algorithm from the boost library so that they can be comapred with the matching algorithm from the SBG library.
In order to run the matching algorithm the following steps must be taken:

* From the SBGLibrary root folder run: `./test/performance/boost/bin/boost-cmp -d ./test/matching/{TEST_MODEL.mo} > {OUTPUT_MODEL}` where `{TEST_MODEL.mo}` is one of the models defined above and `{OUTPUT_MODEL}` is the result output Modelica model.

**Note:** 
The execution will generate a {TEST_MODEL}.log file located in the same folder where the command is called, this file contains detailed information about the different steps taken by the matching algorithm
and also information about the generated graph used by the algorithm. Additionally, a dot file with the definition of the graph is created, to generate a pdf file from the dot file, run:

`dot -Grankdir=LR -Tpdf <PATH_TO_DOT_FILE>`
