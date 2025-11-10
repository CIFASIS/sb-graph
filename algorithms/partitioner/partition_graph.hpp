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

#include <unordered_set>
#include <vector>

#include <sbg/interval.hpp>
#include <sbg/sbg.hpp>

#include "partitioner_params.hpp"
#include "sbg_partitioner_types.hpp"
#include "weighted_sb_graph.hpp"


namespace sbg_partitioner {

constexpr bool sanity_check_enabled = false;


/// @brief Converts a Partition element into a Set.
/// @param partition - A list of SetPiece elements.
/// @return A SBG::LIB::Set representation of the Partition element.
SBG::LIB::Set from_vector(const Partition& partition);


/// @brief Converts a set into a Partition element.
/// @param partition - The set we want to convert set.
/// @return A Partition representation of the set.
Partition to_vector(const SBG::LIB::Set& partition);


PartitionMap
best_initial_partition(
    SBG::LIB::WeightedSBGraph& graph,
    unsigned number_of_partitions,
    const InitialPartitionStrategy strategy,
    bool multithreading_enabled);


/// Returns the connectivity set of a set of edges contained in map1 and map2 of
/// the graph (I mean, edges in CanonSBG::map1()[edge_index] and CanonSBG::map2()[edge_index]).
/// So that, we consider the graph as an undirected graph.
SBG::LIB::Set get_connectivity_set(
    SBG::LIB::SBG& graph,
    const PartitionMap& partitions,
    size_t edge_index);


/// @brief It sorts intervals from smallest to largest from a given partition partition.
/// @param p - partition to be sorted.
void sort_partition_intervals(Partition& p);


/// @brief Pretty print for a given partition.
/// @param partition_map - partition to be written to a string.
/// @return a string that represents the partition as string.
std::string get_output(const PartitionMap& partition_map);


/// @brief It checks if a given partition is valid in terms of:
///
///     1. The partition is a set of disjoint nodes.
///     2. The union of elements is the nodes of the graph.
/// @param graph The graph that have been partitioned.
/// @param partitions_set The obtained partition.
/// @param number_of_partitions The number of partitions
void sanity_check(const SBG::LIB::WeightedSBGraph& graph, PartitionMap& partitions_set, unsigned number_of_partitions);

}
