/*****************************************************************************

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Library.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

/*
* Pretty-prints a DSBGraph returned by SCCGraphBuilder::build(), according to
* the following syntax:
*
* vertex_def := '['(int ',')* int']'
*
* linear_exp := real '*' identifier '+' real
*   | real '*' identifier
*   | identifier
*   | real
*
* linear_exps = '['']' | '['(linear_exp ', ') linear_exp']'
*
* vertex_usage := identifier ' ' linear_exps
*
* range := '['int':'int':'int']'
*
* ranges := '['']' | '['(range ', ')* range']'
*
* edge_def := 'for ' vertex_usage ' -> ' vertex_usage 'endfor;'
*
* graph := vertex_def+ '\n' edge_def*
*
* A graph is a collection of vertices and edges between them. vertex_def and
* edge_def represent these collections. To define an edge vertices are used,
* that's where vertex_usage comes into use.
*
* At the end a converter for SBG is provided, in order to pretty-print them.
*/

#pragma once

#include <boost/foreach.hpp>
#include <ostream>

#include <sbg/util/defs.hpp>
#include <sbg/atom_sbg.hpp>
#include <sbg/sbg.hpp>

namespace SBG {

namespace IO {

// Pretty-printer ---------------------------------------------------------------------------------

struct CompactInterval {
  CompactInterval();
  CompactInterval(int lo, int hi);

  member_class(int, lo);
  member_class(int, hi);
};

std::ostream &operator<<(std::ostream &out, CompactInterval &ci);

typedef std::vector<CompactInterval> CompactIntervals;
std::ostream &operator<<(std::ostream &out, CompactIntervals &ci);

struct VertexDef {
  VertexDef();
  VertexDef(std::string name, CompactIntervals subs_dims);

  member_class(std::string, name);
  member_class(CompactIntervals, subs_dims);

  lt_class(VertexDef);
};

std::ostream &operator<<(std::ostream &out, VertexDef &vd);

typedef std::vector<VertexDef> VertexDefs; 
std::ostream &operator<<(std::ostream &out, VertexDefs &vds);

struct LinearExp {
  LinearExp();
  LinearExp(REAL slope, REAL constant, std::string var);

  member_class(REAL, slope);
  member_class(REAL, constant);
  member_class(std::string, var);

  eq_class(LinearExp);
};

std::ostream &operator<<(std::ostream &out, LinearExp &lm);

typedef std::vector<LinearExp> LinearExps;
std::ostream &operator<<(std::ostream &out, LinearExps &lms);

struct VertexUsage {
  VertexUsage();
  VertexUsage(std::string name, LinearExps subs);

  member_class(std::string, name);
  member_class(LinearExps, subs);

  eq_class(VertexUsage);
};

std::ostream &operator<<(std::ostream &out, VertexUsage &vu);

struct EdgeUsage {
  EdgeUsage();
  EdgeUsage(VertexUsage left, VertexUsage right);

  member_class(VertexUsage, left);
  member_class(VertexUsage, right);

  eq_class(EdgeUsage);
};

std::ostream &operator<<(std::ostream &out, EdgeUsage &eu);

typedef std::vector<EdgeUsage> EdgeUsages;
std::ostream &operator<<(std::ostream &out, EdgeUsages &eus);

typedef std::vector<std::string> Iterators;

// One-dimensional interval
struct Range {
  Range();
  Range(INT begin, INT step, INT end);

  member_class(INT, begin);
  member_class(INT, step);
  member_class(INT, end);

  eq_class(Range);
  lt_class(Range); 
};

std::ostream &operator<<(std::ostream &out, Range &r);

// Multi-dimensional interval
typedef std::vector<Range> Ranges;

std::ostream &operator<<(std::ostream &out, Ranges &rs);

// A collection disjoint of multi-dimensional intervals
// ! The constructor isn't safe: it doesn't check that
// ! intervals are disjoint and of the same dimension
// ! Both is and rs should have the same size
struct EdgeDef {
  EdgeDef();
  EdgeDef(Iterators is, Ranges rs, EdgeUsages edges);

  member_class(Iterators, is);
  member_class(Ranges, rs);
  member_class(EdgeUsages, edges);

  eq_class(EdgeDef);
  lt_class(EdgeDef);
};

std::ostream &operator<<(std::ostream &out, EdgeDef &ed);

typedef std::vector<EdgeDef> EdgeDefs;

std::ostream &operator<<(std::ostream &out, EdgeDefs &eds);

struct GraphIO {
  GraphIO();
  GraphIO(VertexDefs vds, EdgeDefs eds);

  member_class(VertexDefs, vds);
  member_class(EdgeDefs, eds);

  void merge_edges();

  VertexDef &operator[](VertexDef vd);
  EdgeDef &operator[](EdgeDef ed);
};

std::ostream &operator<<(std::ostream &out, GraphIO &grph);

typedef std::vector<VertexUsage> VertexUsages;
std::ostream &operator<<(std::ostream &out, VertexUsages &vrtcs);

// To pretty a representant and its represented vertices
// of a given map
struct ComponentIO {
  ComponentIO();
  ComponentIO(Iterators is, Ranges rs, VertexUsages vrtcs);

  member_class(Iterators, is);
  member_class(Ranges, rs);
  member_class(VertexUsages, vrtcs);
};

std::ostream &operator<<(std::ostream &out, ComponentIO &c);

typedef std::vector<ComponentIO> RMapIO;
std::ostream &operator<<(std::ostream &out, RMapIO &rmap);

} // namespace IO

} // namespace SBG
