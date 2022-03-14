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

#include <string>

#include <sbg/graph_builders/ordinary_graph_builder.hpp>
#include <sbg/util/defs.hpp>
#include <sbg/util/node_finder.hpp>

namespace OG {

OrdinaryGraphBuilder::OrdinaryGraphBuilder(SBG::SBGraph sb_graph) : _sb_graph(sb_graph), _vertex_map() {}

OG::Graph OrdinaryGraphBuilder::build()
{
  OG::Graph graph;

  for (SBG::SetVertexDesc vertex : boost::make_iterator_range(vertices(_sb_graph))) {
    SBG::SetVertex vertex_info = _sb_graph[vertex];
    SBG::Set range = vertex_info.range();
    assert(!range.empty());
    SBG::ORD_INTS iter = range.minElem();
    SBG::ORD_INTS end_iter = range.maxElem();
    while (iter <= end_iter) {
      std::string v_name = getNodeName(vertex_info.name(), iter);
      addVertex(v_name, graph);
      iter = range.nextElem(iter);
    }
  }

  for (SBG::SetEdgeDesc edge : boost::make_iterator_range(edges(_sb_graph))) {
    SBG::SetEdge edge_info = _sb_graph[edge];
    SBG::PWLMap f_map = edge_info.map_f();
    SBG::PWLMap u_map = edge_info.map_u();
    SBG::Set dom = f_map.image();
    SBG::Set image = u_map.image();
    SBG::SetVertex f_vertex = wholeVertex(_sb_graph, dom);
    SBG::SetVertex u_vertex = wholeVertex(_sb_graph, image);
    SBG::ORD_INTS dom_elem = dom.minElem();
    SBG::ORD_INTS dom_end = dom.maxElem();
    SBG::ORD_INTS img_elem = image.minElem();
    SBG::ORD_INTS img_end = image.maxElem();
    while (dom_elem <= dom_end) {
      std::string f_name = getNodeName(f_vertex.name(), dom_elem);
      std::string u_name = getNodeName(u_vertex.name(), img_elem);
      dom_elem = dom.nextElem(dom_elem);
      img_elem = image.nextElem(img_elem);
      int edge_id = num_edges(graph) + 1;
      std::string edge_name = "E_" + std::to_string(edge_id);
      Edge edge(edge_name, edge_id);
      EdgeDesc e;
      bool b;
      boost::tie(e, b) = boost::add_edge(_vertex_map[f_name], _vertex_map[u_name], graph);
      graph[e] = edge;
    }
  }
  return graph;
}

OG::VertexDesc OrdinaryGraphBuilder::addVertex(std::string vertex_name, OG::Graph& graph)
{
  Vertex V(vertex_name, num_vertices(graph) + 1);
  OG::VertexDesc v = boost::add_vertex(graph);
  _vertex_map[vertex_name] = v;
  graph[v] = V;
  return v;
}

std::string OrdinaryGraphBuilder::getNodeName(std::string name, SBG::ORD_INTS elems)
{
  std::stringstream v_name;
  v_name << name << "_";
  unsigned int d_i = 0;
  unsigned int d_size = elems.size();
  for (SBG::INT d : elems) {
    v_name << d << ((++d_i < d_size) ? "_" : "");
  }
  return v_name.str();
}

}  // namespace OG
