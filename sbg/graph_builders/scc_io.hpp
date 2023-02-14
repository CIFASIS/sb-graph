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
* edge_def := 'for ' vertex_usage ' -> ' vertex_usage ';'
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
#include <sbg/sbg.hpp>

namespace SBG {

namespace IO {

// Pretty-printer ---------------------------------------------------------------------------------

typedef std::vector<int> Ints;
std::ostream &operator<<(std::ostream &out, Ints &is);

struct VertexDef {
  VertexDef();
  VertexDef(std::string name, Ints subs_dims);

  member_class(std::string, name);
  member_class(Ints, subs_dims);
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
};

std::ostream &operator<<(std::ostream &out, LinearExp &lm);

typedef std::vector<LinearExp> LinearExps;
std::ostream &operator<<(std::ostream &out, LinearExps &lms);

struct VertexUsage {
  VertexUsage();
  VertexUsage(std::string name, LinearExps subs);

  member_class(std::string, name);
  member_class(LinearExps, subs);
};

std::ostream &operator<<(std::ostream &out, VertexUsage &vu);

typedef std::vector<std::string> Iterators;

// One-dimensional interval
struct Range {
  Range();
  Range(INT begin, INT step, INT end);

  member_class(INT, begin);
  member_class(INT, step);
  member_class(INT, end);
 
  bool operator<(const Range &other) const;
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
  EdgeDef(Iterators is, Ranges rs, VertexUsage vb, VertexUsage vf);

  member_class(Iterators, is);
  member_class(Ranges, rs);
  member_class(VertexUsage, vb);
  member_class(VertexUsage, vf);
};

std::ostream &operator<<(std::ostream &out, EdgeDef &ed);

typedef std::vector<EdgeDef> EdgeDefs;

std::ostream &operator<<(std::ostream &out, EdgeDefs &eds);

struct SCCGraphIO {
  SCCGraphIO();
  SCCGraphIO(VertexDefs vds, EdgeDefs eds);

  member_class(VertexDefs, vds);
  member_class(EdgeDefs, eds);
};

std::ostream &operator<<(std::ostream &out, SCCGraphIO &grph);

// Converter --------------------------------------------------------------------------------------

struct SCCConverter {
  SCCConverter();
  SCCConverter(DSBGraph dg);

  member_class(DSBGraph, dg);

  SCCGraphIO convert();

  private:
  Range convert_interval(Interval i);
  Ranges convert_multi_interval(MultiInterval mi);
  VertexDef convert_vertex(DSetVertexDesc vd);

  Iterators create_iterators(MultiInterval mi);
  Ranges create_ranges(MultiInterval mi);
  LinearExps create_exp(MultiInterval mi);
  EdgeDefs convert_edge(DSetEdgeDesc ed);
};

} // namespace IO

} // namespace SBG
