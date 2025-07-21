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

#include <sbg/sbg.hpp>

#include "partition_graph.hpp"


namespace sbg_partitioner {

namespace internal {

using ec_ic = std::pair<SBG::LIB::Set , SBG::LIB::Set>;

ec_ic compute_EC_IC(
    const Partition& partition,
    const SBG::LIB::SetPiece& nodes,
    const SBG::LIB::PWMap& map_1,
    const SBG::LIB::PWMap& map_2,
    const SBG::LIB::SetAF& set_fact);

}

class CostMatrix {
public:
    CostMatrix(const SBG::LIB::WeightedSBGraph& graph, PartitionMap& partitions, const SBG::LIB::SetAF& set_fact);

    PartitionMap partitions() const;
    void update_partitions(PartitionMap& partitions, std::optional<std::vector<size_t>> modified_partitions = std::nullopt);

    SBG::LIB::Set get_ec_by_partition_id(unsigned partition_id); // non-const since _cost_by_partition may be updated

    SBG::LIB::Set get_ec_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes); // non-const since _ec_cost_by_interval and _ic_cost_by_interval may be updated
    SBG::LIB::Set get_ic_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes); // non-const since _ec_cost_by_interval and _ic_cost_by_interval may be updated

private:
    const SBG::LIB::WeightedSBGraph& _graph; // read-only members
    PartitionMap _partitions;
    const SBG::LIB::SetAF& _set_fact;

    std::vector<std::pair<SBG::LIB::Set, SBG::LIB::Set>> _cost_by_partition;
    std::vector<std::unordered_map<SBG::LIB::SetPiece, SBG::LIB::Set, SBG::LIB::SetPieceHash>> _ec_cost_by_interval;
    std::vector<std::unordered_map<SBG::LIB::SetPiece, SBG::LIB::Set, SBG::LIB::SetPieceHash>> _ic_cost_by_interval;

    void initialize();

};

}