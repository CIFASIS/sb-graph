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

}