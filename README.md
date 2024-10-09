# Set-Based Graph Library

Set-based graphs (SB-Graphs) are graphs in which the vertices and edges are grouped in sets allowing sometimes a compact representation. When the compact representation is feasible, it is possible to develop new versions of traditional algorithms that take advantage of the structure of SB-Graphs, with a cost that is independent on the size of the sets of vertices and edges. Current algorithms for SB-Graphs:

  * Connected Components
  * Matching
  * Strongly Connected Components (SCC)
  * Topological Sort
  * Minimum Vertex Cut Set

This library defines data structures to represent SB-Graphs, and implements the aforementioned algorithms. The related publications can be used as documentation of the code.

This new approach was used in the flatter and causalization stage of the Modelica compiler, ModelicaCC (https://github.com/CIFASIS/modelicacc). Nevertheless, many fields could benefit of its use.

## Related Publications

[1] Denise Marzorati, Joaquín Fernández, and Ernesto Kofman. 2024. Efficient Matching in Large DAE Models. ACM Trans. Math. Softw. Just Accepted (June 2024). https://doi.org/10.1145/3674831

[2] Denise Marzorati, Joaquin Fernández, Ernesto Kofman. Connected Components in Undirected Set--Based Graphs. Applications in Object--Oriented Model Manipulation Applied Mathematics and Computation, Volume 418, 2022, 126842,ISSN 0096-3003, https://doi.org/10.1016/j.amc.2021.126842.

[3] Ernesto Kofman, Joaquín Fernández, Denise Marzorati. Compact sparse symbolic Jacobian computation in large systems of ODEs Applied Mathematics and Computation, Volume 403, 2021, 126181, ISSN 0096-3003, https://doi.org/10.1016/j.amc.2021.126181.

[4] Pablo Zimmermann, Joaquin Fernandez, Ernesto Kofman.
**Set-based graph methods for fast equation sorting in large DAE systems**
 EOOLT '19: Proceedings of the 9th International Workshop on Equation-based Object-oriented Modeling Languages and Tools 2019

## Installation

These are generic installation instructions.

## Dependencies

In order to be able to install and compile SBG Library, 
the following  dependencies must be installed: 

  * autoconf 2.69 (avoid 2.71)
  * boost1.81 (or later)
  * cmake
  * g++
  * make
  * rapidjson-dev
  * doxygen (optional)     

## Basic Installation

The simplest way to compile this package is:

  1. `cd` to the directory containing the package's source code and type
     `autoconf` to generate the configuration scripts.
  
  2. Type `./configure` to run the configuration script. 
      
  3. Type `make` to compile the library `libsbgraph.a`

  4. Type `sudo make install` to install the library and header files in the 
    default installation folders.
    The default installation folders are:
      * prefix=/usr/local
      * includedir=/usr/local/include
      * libdir=/usr/local/lib

  5. You can remove the generated library and object files from the
     source code directory by typing `make clean`.  

## Makefile options

The makefile script accepts the following options:

  * MODE = <Debug|Release> 	When set to Debug (default), adds the compiler's debug flags.

  * prefix = <Installation prefix path> 	Set the prefix installation path, default: /usr/local.

  * exec_prefix = <Binaries installation path> 	Set the binaries installation path, default: /usr/local.

  * includedir = <Headers installation path> 	Set the header installation path, default: /usr/local/incldue.

  * libdir = <Library installation path> 	Set the library installation path, default: /usr/local/lib.

## Makefile targets

The makefile script accepts the following targets:

  * test: 		Builds and run integration and unit tests.

  * doc:      Builds the documentation.

## SBG library

The SBG library is composed by four main modules:

  * ast

  * parser

  * eval

  * sbg

The first three were developed to allow for more user-friendly input. The first
three were developed to allow for more user-friendly input, and the last one
contains all the logical implementation of structures and operations.

## Examples:			   

Examples of SBG programs are presented in the /test folder with the '.test'
extension. The bin/sbg-parser and bin/sbg-eval binaries accept these files as
their input. Also, the `make test` command in the root directory will execute
all of the existing tests.

## Licensing

Please see the file called LICENSE.

## Bug Reporting

Report bugs to: marzorati@cifasis-conicet.gov.ar or fernandez@cifasis-conicet.gov.ar

# Changelog

## [3.0.0] - 2024-10-09
### Added
- Implemented Ordered Sets of one-dimensional, compact MDIs.
- Implemented novel versions of Maximum Matching, SCC and Topological Sort of SBG algorithms.
- Implemented Cut Set SBG algorithm for Tearing.
- Implemented JSON output for client code.
