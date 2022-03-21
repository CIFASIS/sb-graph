## Set-based graphs

Set-based graphs (SB-Graphs) are regular graphs in which the vertices and edges are grouped in sets allowing
sometimes a compact representation. When the compact representation is feasible, it is possible to develop 
new versions of traditional algorithms that take advantage of the structure of SB-Graphs, with a cost that 
is independent on the size of the sets of vertices and edges. Current algorithms for SB-Graphs:

  * Connected Components
  * Matching

This library defines data structures to represent SB-Graphs, and implements the aforementioned algorithms. The
related publications can be used as documentation of the code.

This new approach was used in the flatter and causalization stage of the Modelica compiler, ModelicaCC 
(https://github.com/CIFASIS/modelicacc). Nevertheless, many fields could benefit of its use.

## Related Publications

[1] Denise Marzorati, Joaquin Fernández, Ernesto Kofman.
**Connected Components in Undirected Set--Based Graphs. Applications in Object--Oriented Model Manipulation**
Applied Mathematics and Computation, Volume 418, 2022, 126842,ISSN 0096-3003, https://doi.org/10.1016/j.amc.2021.126842.

[2] Ernesto Kofman, Joaquín Fernández, Denise Marzorati.
**Compact sparse symbolic Jacobian computation in large systems of ODEs**
Applied Mathematics and Computation, Volume 403, 2021, 126181, ISSN 0096-3003, https://doi.org/10.1016/j.amc.2021.126181.

## Dependencies

In order to be able to install and compile sb-graph, 
the following  dependencies must be installed: 

    * autoconf
    * boost1.71
    * cmake
    * doxygen
    * g++
    * make

## Basic Installation

The simplest way to compile this package is:

  1. `cd` to the directory containing the package's source code and type
     `autoconf` to generate the configuration scripts.
  
  2. Type `./configure` to run the configuration script.
  
  3. Type `make` to compile all the binaries. The different tools are located 
     in the bin folder. 

  4. You can remove the program binaries and object files from the
     source code directory by typing `make clean`.
     
## Licensing

Please see the file called LICENSE.

## Bug Reporting

Report bugs to: fernandez@cifasis-conicet.gov.ar
