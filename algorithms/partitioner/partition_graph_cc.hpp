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

#include <unordered_set>
#include <vector>

#include <sbg/interval.hpp>
#include <sbg/sbg.hpp>

#include "communication_cost.hpp"
#include "partitioner_params.hpp"
#include "sbg_partitioner_types.hpp"
#include "weighted_sb_graph.hpp"

namespace sbg_partitioner {

namespace using_cc {


int run_bisection(
    const SBG::LIB::WeightedSBGraph& graph,
    CommunicationCostCC& cost_matrix,
    SetPointers& partition_a,
    SetPointers& partition_b,
    const SBG::LIB::PWMap& cc_map,
    unsigned LMin,
    unsigned LMax);

void bisection(
    const SBG::LIB::WeightedSBGraph& graph,
    CommunicationCostCC& cost_matrix,
    const SetPointers& sorted_nodes,
    SetPointers& partition_a,
    SetPointers& partition_b,
    const SBG::LIB::PWMap& cc_map,
    unsigned LMin,
    unsigned LMax);

PartitionMap rebuild_partitions(
    const SetPointers& new_nodes,
    const std::vector<SetPointers>& partitions);


}

}