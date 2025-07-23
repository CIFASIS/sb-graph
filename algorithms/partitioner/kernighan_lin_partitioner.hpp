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

#pragma once

#include <set>
#include <string>

#include "partition_graph.hpp"

namespace sbg_partitioner {

/**
 * @brief Entry point of the algorithm. It takes a graph, an initial partition
 * and the imbalance epsilon which indicates the allowed disbalance. It will run
 * Kernighan-Lin algorithm heuristic to the given partitions.
 * 
 * @param graph - the provided weighted sb graph.
 * @param partitions - the initial partition.
 * @param imbalance_epsilon - float between 0 and 1  that indicates the allowed disbalance (0 means no disbalance).
 */
void kl_sbg_imbalance_partitioner(
    const SBG::LIB::WeightedSBGraph& graph,
    PartitionMap& partitions,
    const float imbalance_epsilon);


/**
 * @brief pretty print for a sb graph.
 */
std::string get_pretty_sb_graph(const SBG::LIB::SBG& g);


struct KLBipartResult {
    Partition A;
    Partition B;
    int gain;
};

std::ostream& operator<<(std::ostream& os, const KLBipartResult& result);


struct GainObjectImbalance {
    size_t a_idx;
    size_t b_idx;
    int gain;
    SBG::LIB::Set ec_nodes_a;
    SBG::LIB::Set ic_nodes_a;
    size_t size_a;
    SBG::LIB::Set ec_nodes_b;
    SBG::LIB::Set ic_nodes_b;
    size_t size_b;

    bool operator==(const GainObjectImbalance& gain_2) const
    {
        return a_idx == gain_2.a_idx
            and b_idx == gain_2.b_idx
            and gain == gain_2.gain
            and ec_nodes_a == gain_2.ec_nodes_a
            and ic_nodes_a == gain_2.ic_nodes_a
            and size_a == gain_2.size_a
            and ec_nodes_b == gain_2.ec_nodes_b
            and ec_nodes_b == gain_2.ec_nodes_b
            and size_b == gain_2.size_b;
    }
};

std::ostream& operator<<(std::ostream& os, const GainObjectImbalance& gain);


template<typename G>
struct GainObjectComparatorTemplate {
    bool operator()(const G& gain_1, const G& gain_2) const
    {
        return gain_1.gain >= gain_2.gain;
    }
};

using GainObjectImbalanceComparator = GainObjectComparatorTemplate<GainObjectImbalance>;

using CostMatrixImbalance = std::set<GainObjectImbalance, GainObjectImbalanceComparator>;

std::ostream& operator<<(std::ostream& os, const CostMatrixImbalance& cost_matrix);


struct kl_sbg_partitioner_result
{
    size_t i;
    size_t j;
    int gain;
    Partition A;
    Partition B;
};

std::ostream& operator<<(std::ostream& os, const kl_sbg_partitioner_result& result);

}