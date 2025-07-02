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

void kl_sbg_imbalance_partitioner(
    const SBG::LIB::WeightedSBGraph& graph,
    PartitionMap& partitions,
    const float imbalance_epsilon,
    SBG::LIB::SetAF& set_fact,
    SBG::LIB::MapAF& map_fact);


std::string get_pretty_sb_graph(const SBG::LIB::SBG& g);


struct KLBipartResult {
    Partition A;
    Partition B;
    int gain;
};

std::ostream& operator<<(std::ostream& os, const KLBipartResult& result);


struct GainObjectImbalance {
    size_t i;
    size_t j;
    int gain;
    SBG::LIB::Set ec_nodes_i;
    SBG::LIB::Set ic_nodes_i;
    size_t size_i;
    SBG::LIB::Set ec_nodes_j;
    SBG::LIB::Set ic_nodes_j;
    size_t size_j;

    bool operator==(const GainObjectImbalance& gain_2) const
    {
        return i == gain_2.i
            and j == gain_2.j
            and gain == gain_2.gain
            and ec_nodes_i == gain_2.ec_nodes_i
            and ic_nodes_i == gain_2.ic_nodes_i
            and size_i == gain_2.size_i
            and ec_nodes_j == gain_2.ec_nodes_j
            and ec_nodes_j == gain_2.ec_nodes_j
            and size_j == gain_2.size_j;
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