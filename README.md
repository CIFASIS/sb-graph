# Set Based Graph 1.0

Set--Based Graphs (SBGs) are regular graphs in which the vertices and edges are grouped in sets allowing
sometimes a compact representation. SBG were introduced and formally defined [here](https://dl.acm.org/doi/abs/10.1145/3365984.3365991)
Given the presence of repetitive structures in 
graphs, the vertices and edges can be are grouped in different sets with a compact representation
by intension.
  
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
     The default installation folders are:
       * prefix=/usr/local
       * includedir=/usr/local/include
       * libdir=/usr/local/lib
      
  3. Type `make` to compile the library `libsbgraph.a`

  4. Type `make install` to install the library and header files in the 
    installation folders defined above.

  5. You can remove the generated library and object files from the
     source code directory by typing `make clean`.  

## Examples:			   
The [examples](https://github.com/CIFASIS/sb-graph/tree/sb-graph-dev/examples) folder contains three simple use cases of the **matching algorithm** defined in the library, including the use of the [IO](https://github.com/CIFASIS/sb-graph/tree/sb-graph-dev/sbg/graph_builders) interface defined for the matching structures.
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
