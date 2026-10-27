/**
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

#include "weighted_sb_graph.hpp"

using namespace std;

namespace SBG {

namespace LIB {

WeightedSBGraph::WeightedSBGraph(WeightedSBGraph& graph)
    : SBG(graph), _node_weights(graph.get_node_weights()), _edge_costs(graph.get_edge_costs())
{
}

WeightedSBGraph::WeightedSBGraph(WeightedSBGraph&& graph)
    : SBG(graph), _node_weights(graph.get_node_weights()), _edge_costs(graph.get_edge_costs())
{
}

WeightedSBGraph::WeightedSBGraph(const Set& V, const PWMap& Vmap, const PWMap& map1, const PWMap& map2, const PWMap& Emap,
                                 const PWMap& subEmap)
    : SBG(V, Vmap, map1, map2, Emap, subEmap)
{
}

WeightedSBGraph::~WeightedSBGraph() = default;

void WeightedSBGraph::set_node_weights(NodeWeight& node_weights) { _node_weights = std::move(node_weights); }

NodeWeight WeightedSBGraph::get_node_weights() const { return _node_weights; }

void WeightedSBGraph::set_node_weight(const Set& node_set, int weight) { _node_weights[node_set] = weight; }

int WeightedSBGraph::get_node_weight(const Set& node_set) const { return _node_weights.at(node_set); }

void WeightedSBGraph::set_edge_costs(EdgeCost& edge_costs) { _edge_costs = std::move(edge_costs); }

EdgeCost WeightedSBGraph::get_edge_costs() const { return _edge_costs; }

void WeightedSBGraph::set_edge_cost(const Set& edge_set, unsigned cost) { _edge_costs[edge_set] = cost; }

unsigned WeightedSBGraph::get_edge_cost(const Set& edge_set) const { return _edge_costs.at(edge_set); }

ostream& operator<<(ostream& os, const WeightedSBGraph& graph)
{
  os << SBG(graph);

  os << "node weight = << ";
  for (const auto& [set, weight] : graph.get_node_weights()) {
    os << set << " ↦ " << weight << ", ";
  }

  os << "\b >>" << endl;

  os << "edge costs = << ";
  for (const auto& [set, weight] : graph.get_edge_costs()) {
    os << set << " ↦ " << weight << ", ";
  }

  os << "\b >>" << endl;

  return os;
}

}  // namespace LIB

}  // namespace SBG
