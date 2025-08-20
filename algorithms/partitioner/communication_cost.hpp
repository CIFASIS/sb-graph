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

#include <list>
#include <unordered_map>
#include <vector>

#include <sbg/sbg.hpp>

#include "sbg_partitioner_types.hpp"
#include "weighted_sb_graph.hpp"


namespace sbg_partitioner {

class CommunicationCost {
public:
    CommunicationCost(const SBG::LIB::WeightedSBGraph& graph, PartitionMap partitions);

    /**
     * @brief After partitions are modified, external and internal cost must be updated and this function takes care of it.
     * If `modified_partitions` is provided, only modified partitions will be updated to speed up the process.
     * 
     * @param partitions - new partition of the graph nodes.
     * @param modified_partitions - [optional] partitions that were updated.
     */
    void update_partitions(PartitionMap& partitions, std::optional<std::reference_wrapper<const std::list<size_t>>> modified_partitions = std::nullopt);

    /**
     * It returns the edges that communicate nodes in partition `partition_id` with others.
     * 
     * @param partition_id - id or index of a partition.
     * 
     * @return External edges of the given partition.
     * @note These values are pre-computed when the object is created or partitions are updated.
     */
    SBG::LIB::Set get_ec_by_partition_id(unsigned partition_id); // non-const since _cost_by_partition may be updated

    /**
     * It returns the edges that communicate the set piece nodes in partition `partition_id` with other partitions.
     * 
     * @param partition_id - id or index of a partition.
     * @param nodes - the set piece of `partition_id` that we want to compute external edges.
     * 
     * @return External edges of the given set piece.
     * @note the member function is non-const since _ec_cost_by_interval and _ic_cost_by_interval may be updated to prevent to be recomputed.
     */

    SBG::LIB::Set get_ec_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes);

    /**
     * It returns the edges that communicate the set piece nodes with other nodes in partition `partition_id`.
     * 
     * @param partition_id - id or index of a partition.
     * @param nodes - the set piece of `partition_id` that we want to compute internal edges.
     * 
     * @return Internal edges of the given set piece.
     * @note the member function is non-const since _ec_cost_by_interval and _ic_cost_by_interval may be updated to prevent to be recomputed.
     */
    SBG::LIB::Set get_ic_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes);

private:
    const SBG::LIB::WeightedSBGraph& _graph; // read-only members
    PartitionMap _partitions;

    std::unordered_map<SBG::LIB::SetPiece, SBG::LIB::Set, SBG::LIB::SetPieceHash> _communication_by_set_piece;
    std::vector<std::pair<SBG::LIB::Set, SBG::LIB::Set>> _cost_by_partition;
    std::vector<std::unordered_map<SBG::LIB::SetPiece, SBG::LIB::Set, SBG::LIB::SetPieceHash>> _ec_cost_by_interval;
    std::vector<std::unordered_map<SBG::LIB::SetPiece, SBG::LIB::Set, SBG::LIB::SetPieceHash>> _ic_cost_by_interval;

    void initialize();

};

/**
 * @brief Once communication cost object is created, it can be saved by calling this function to be used in the future.
*/
void set_communication_cost(CommunicationCost& cost_matrix);

/**
 * @brief Global communication cost object, saved to prevent recomputing.
*/
CommunicationCost& get_communication_cost();

}