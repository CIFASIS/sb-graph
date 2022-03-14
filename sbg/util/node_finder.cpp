/*****************************************************************************

 This file is part of QSS Solver.

 QSS Solver is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 QSS Solver is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with QSS Solver.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <sbg/util/node_finder.hpp>

using namespace std;

namespace SBG {

SBG::EdgeIt findSetEdge(SBG::SBGraph& graph, SBG::Set matched)
{
  // Find the set-vertex where matched subset is included
  SBG::EdgeIt ei_start, ei_end;
  boost::tie(ei_start, ei_end) = edges(graph);

  for (; ei_start != ei_end; ++ei_start) {
    SBG::SetEdge e = graph[*ei_start];
    SBG::PWLMap map = e.map_f();
    SBG::Set dom = map.wholeDom();
    SBG::Set inter = dom.cap(matched);
    if (!inter.empty()) {
      return ei_start;
    }
  }
  // A given subset should be included in one of the graph vertex, this should never happen.
  assert(false);
  return ei_start;
}

SBG::VertexIt findSetVertex(SBG::SBGraph& graph, SBG::Set matched)
{
  // Find the set-vertex where matched subset is included
  SBG::VertexIt vi_start, vi_end;
  boost::tie(vi_start, vi_end) = vertices(graph);

  for (; vi_start != vi_end; ++vi_start) {
    SBG::SetVertex v = graph[*vi_start];
    SBG::Set vs = v.range();
    SBG::Set inter = vs.cap(matched);
    if (!inter.empty()) {
      return vi_start;
    }
  }
  // A given subset should be included in one of the graph vertex, this should never happen.
  assert(false);
  return vi_start;
}

SBG::SetVertex wholeVertex(SBG::SBGraph& graph, SBG::Set matched_subset)
{
  // Find the set-vertex where the matched subset is included
  SBG::VertexIt matched_vertex = findSetVertex(graph, matched_subset);
  return graph[*matched_vertex];
}

}  // namespace SBG
