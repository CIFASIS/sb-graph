# Set-Based Graph Library

Set-based graphs (SB-Graphs) are regular graphs in which the vertices and edges are grouped in sets allowing sometimes a compact representation. When the compact representation is feasible, it is possible to develop new versions of traditional algorithms that take advantage of the structure of SB-Graphs, with a cost that is independent on the size of the sets of vertices and edges. Current algorithms for SB-Graphs:

  * Connected Components
  * Matching

This library defines data structures to represent SB-Graphs, and implements the aforementioned algorithms. The related publications can be used as documentation of the code.

This new approach was used in the flatter and causalization stage of the Modelica compiler, ModelicaCC (https://github.com/CIFASIS/modelicacc). Nevertheless, many fields could benefit of its use.

## Related Publications

[1] Denise Marzorati, Joaquin Fernández, Ernesto Kofman. Connected Components in Undirected Set--Based Graphs. Applications in Object--Oriented Model Manipulation Applied Mathematics and Computation, Volume 418, 2022, 126842,ISSN 0096-3003, https://doi.org/10.1016/j.amc.2021.126842.

[2] Ernesto Kofman, Joaquín Fernández, Denise Marzorati. Compact sparse symbolic Jacobian computation in large systems of ODEs Applied Mathematics and Computation, Volume 403, 2021, 126181, ISSN 0096-3003, https://doi.org/10.1016/j.amc.2021.126181.

[3] Pablo Zimmermann, Joaquin Fernandez, Ernesto Kofman.
**Set-based graph methods for fast equation sorting in large DAE systems**
 EOOLT '19: Proceedings of the 9th International Workshop on Equation-based Object-oriented Modeling Languages and Tools 2019

## Installation

These are generic installation instructions.

## Dependences

In order to be able to install and compile SBG Library, 
the following  dependencies must be installed: 

  * autoconf
  * boost1.71
  * cmake
  * g++
  * make     

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

## Examples:			   
The [examples](https://github.com/CIFASIS/sb-graph/tree/sb-graph-dev/examples) folder contains three simple use cases of the **matching algorithm** defined in the library, including the use of the [IO](https://github.com/CIFASIS/sb-graph/blob/sb-graph-dev/sbg/graph_builders/matching_io.hpp) interface defined for the matching structures.
All the examples are based on simple Modelica models, for instance [advection.c](https://github.com/CIFASIS/sb-graph/blob/sb-graph-dev/examples/advection/advection.c) shows a simple use case for the following model:   

```
 // Simple use example of Advection model.
 model advection
   parameter Real alpha=0.5,mu=1000;
   constant Integer N = 20000;
   Real u[N];
 initial algorithm
   for i in 1:N/3 loop
     u[i]:=1;
   end for;
 equation
   der(u[1])=(-u[1]+1)*N-mu*u[1]*(u[1]-alpha)*(u[1]-1);
   for j in 2:N loop
     der(u[j])=(-u[j]+u[j-1])*N-mu*u[j]*(u[j]-alpha)*(u[j]-1);
    end for;
 end advection; 
```

## Licensing

Please see the file called LICENSE.

## Bug Reporting

Report bugs to: marzorati@cifasis-conicet.gov.ar or fernandez@cifasis-conicet.gov.ar
