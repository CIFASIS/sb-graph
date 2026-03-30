/*****************************************************************************

 This file is part of SBG Partitioner.

 SBG Partitioner is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Partitioner is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Partitioner.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#pragma once

#include <sbg/sbg.hpp>

#include "weighted_sb_graph.hpp"

namespace sbg_partitioner {


/// Takes a path to a json file, reads it and then builds a sb graph with
/// a node for each access to a variable and an edge for each connection
/// between variables.
/// If a variable appears on the left and on the right side, an edge is created.
SBG::LIB::WeightedSBGraph build_sb_graph(const std::string& filename, bool compact_maps = true);


/// Ad hoc function to get pre image of an expression from its image.
/// @param image_interval  Image we want to get the pre image from
/// @param expression  Expression to get the pre image
/// @return pre image as an interval
SBG::LIB::Interval get_pre_image(const SBG::LIB::Interval& image_interval, const SBG::LIB::LExp& expression);


/// Takes a graph and a set of nodes and return a set of nodes connected with the mentioned set.
/// If a node is connected at least by one edge with one node in the set, it will be in the returned
/// set.
/// @param graph the graph where we are looking for connections.
/// @param node set of nodes we want to know its connections.
/// @return a set of nodes connected to the function parameter.
SBG::LIB::Set get_adjacents(const SBG::LIB::SBG& graph, const SBG::LIB::Set& node);


/// Takes a set piece and calculate its size of the intervals. E.g [1:10] has 10 elements,
/// {[1:10], [1:10]} has 100 elements.
/// @param node input set we want to calculate the size.
/// @return the number of elements
unsigned get_node_size(const SBG::LIB::SetPiece& node, const SBG::LIB::NodeWeight& node_weight);


/// Takes each set piece and calculates its size, it returns the sum of them
unsigned get_node_size(const SBG::LIB::Set& node, const SBG::LIB::NodeWeight& node_weight);


/// Takes each set piece of the partition and calculates its size, it returns the sum of them
unsigned get_partition_size(const std::vector<SBG::LIB::SetPiece>& node, const SBG::LIB::NodeWeight& node_weight);


/// Takes a set of edges and compute its cost.
/// @param edges - input set edges we want to calculate the size.
/// @return the cost of edges (number of elements * communication cost).
unsigned get_edge_set_cost(const SBG::LIB::Set& edges, const SBG::LIB::EdgeCost& edge_cost);


/// Takes a set piece of edges and compute its cost.
/// @param edges - input set piece edges we want to calculate the size.
/// @return the cost of edges (number of elements * communication cost).
unsigned get_edge_set_cost(const SBG::LIB::SetPiece& node, const SBG::LIB::EdgeCost& edge_cost);


/// Flattens a set of nodes according to the graph nodes.
/// @param edges - input set edges we want to calculate the size.
/// @return the cost of edges (number of elements * communication cost).
void flatten_set(SBG::LIB::Set &set, const SBG::LIB::WeightedSBGraph& graph);


SBG::LIB::WeightedSBGraph create_air_conditioners_graph();


SBG::LIB::WeightedSBGraph create_air_conditioners_with_controller_graph(int size);


/// It returns the edge cost or node weight of the input set. It looks for a key in cost that intersects
/// the input set, and returns its value. If no key intersects the input set, it will return 1.
/// @param set input set we want to know the cost or weight/
/// @param costs hashtable with set/costs.
/// @return the cost of set.
template<typename T>
int get_set_cost(const SBG::LIB::SetPiece& set_piece, const T& costs)
{
  int weight = 1;
  auto set = SBG::LIB::SET_FACT.createSet(set_piece);
  for (const auto& [cost_set, w] : costs) {
    if (set.intersection(cost_set).size() > 0) {
      weight = costs.at(cost_set);
    }
  }

  return weight;
}


size_t get_set_size(const SBG::LIB::Set& set);


std::pair<SBG::LIB::Set, SBG::LIB::Set> cut_interval_by_dimension(
    SBG::LIB::Set& set_piece,
    const SBG::LIB::NodeWeight& node_weight,
    std::size_t size);

std::pair<SBG::LIB::SetPiece, SBG::LIB::SetPiece> cut_interval(const SBG::LIB::SetPiece& interval, int cut_value);

} // namespace sbg-partitioner
