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
#include <mutex>
#include <unordered_map>
#include <vector>

#include <sbg/sbg.hpp>

#include "sbg_partitioner_types.hpp"
#include "weighted_sb_graph.hpp"


namespace sbg_partitioner {


class ICommunicationCost {
public:
    ICommunicationCost() = default;

    virtual ~ICommunicationCost() = default;

    /**
     * @brief After partitions are modified, external and internal cost must be updated and this function takes care of it.
     * If `modified_partitions` is provided, only modified partitions will be updated to speed up the process.
     * 
     * @param partitions - new partition of the graph nodes.
     * @param modified_partitions - [optional] partitions that were updated.
     */
    virtual void update_partitions(PartitionMap& partitions, std::optional<std::reference_wrapper<const std::list<size_t>>> modified_partitions = std::nullopt) = 0;

    /**
     * It returns the edges that communicate nodes in partition `partition_id` with others.
     * 
     * @param partition_id - id or index of a partition.
     * 
     * @return External edges of the given partition.
     * @note These values are pre-computed when the object is created or partitions are updated.
     */
    virtual SBG::LIB::Set get_ec_by_partition_id(unsigned partition_id) = 0; // non-const since _cost_by_partition may be updated

    /**
     * It returns the edges that communicate the set piece nodes in partition `partition_id` with other partitions.
     * 
     * @param partition_id - id or index of a partition.
     * @param nodes - the set piece of `partition_id` that we want to compute external edges.
     * 
     * @return External edges of the given set piece.
     * @note the member function is non-const since _ec_cost_by_interval and _ic_cost_by_interval may be updated to prevent to be recomputed.
     */

    virtual SBG::LIB::Set get_ec_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes) = 0;

    /**
     * It returns the edges that communicate the set piece nodes with other nodes in partition `partition_id`.
     * 
     * @param partition_id - id or index of a partition.
     * @param nodes - the set piece of `partition_id` that we want to compute internal edges.
     * 
     * @return Internal edges of the given set piece.
     * @note the member function is non-const since _ec_cost_by_interval and _ic_cost_by_interval may be updated to prevent to be recomputed.
     */
    virtual SBG::LIB::Set get_ic_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes) = 0;
};


/// Communication cost pointer
typedef std::unique_ptr<ICommunicationCost> CommunicationCostPtr;

/**
 * The actual communication cost class.
 */
class CommunicationCost : public ICommunicationCost {
public:
    CommunicationCost(const SBG::LIB::WeightedSBGraph& graph, PartitionMap partitions);

    ~CommunicationCost() = default;

    void update_partitions(PartitionMap& partitions, std::optional<std::reference_wrapper<const std::list<size_t>>> modified_partitions = std::nullopt) override;

    SBG::LIB::Set get_ec_by_partition_id(unsigned partition_id)  override; // non-const since _cost_by_partition may be updated

    SBG::LIB::Set get_ec_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes)  override;

    SBG::LIB::Set get_ic_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes) override;

private:
    const SBG::LIB::WeightedSBGraph& _graph; // read-only members
    PartitionMap _partitions;

    // since communication is independent from the partitions, we can share it between many objects
    static std::unordered_map<SBG::LIB::SetPiece, SBG::LIB::Set, SBG::LIB::SetPieceHash> _communication_by_set_piece;
    std::vector<std::pair<SBG::LIB::Set, SBG::LIB::Set>> _cost_by_partition;
    std::vector<std::unordered_map<SBG::LIB::SetPiece, SBG::LIB::Set, SBG::LIB::SetPieceHash>> _ec_cost_by_interval;
    std::vector<std::unordered_map<SBG::LIB::SetPiece, SBG::LIB::Set, SBG::LIB::SetPieceHash>> _ic_cost_by_interval;

    void initialize();
    std::pair<SBG::LIB::Set, SBG::LIB::Set> compute_ec_ic(unsigned partition_id, const SBG::LIB::SetPiece& nodes);
};


/**
 * Communication cost to run optimization using multithreading.
 */
class CommunicationCostSync : public ICommunicationCost {
public:
    CommunicationCostSync(const SBG::LIB::WeightedSBGraph& graph, PartitionMap partitions);

    ~CommunicationCostSync() = default;

    void update_partitions(PartitionMap& partitions, std::optional<std::reference_wrapper<const std::list<size_t>>> modified_partitions = std::nullopt) override;

    SBG::LIB::Set get_ec_by_partition_id(unsigned partition_id) override; // non-const since _cost_by_partition may be updated

    SBG::LIB::Set get_ec_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes) override;

    SBG::LIB::Set get_ic_by_interval(unsigned partition_id, const SBG::LIB::SetPiece& nodes) override;

private:
    CommunicationCost _comm_cost;
    std::mutex _mutex;
};


CommunicationCostPtr create_communication_cost(const SBG::LIB::WeightedSBGraph& graph, PartitionMap partitions, bool multithreading_enabled);

/**
 * @brief Once communication cost object is created, it can be saved by calling this function to be used in the future.
*/
void set_communication_cost(CommunicationCostPtr&& cost_matrix);

/**
 * @brief Global communication cost object, saved to prevent recomputing.
*/
ICommunicationCost& get_communication_cost();

}