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

#include <unordered_map>

 #include "communication_cost.hpp"
 #include "partition_graph.hpp"


using namespace std;

using namespace SBG::LIB;

namespace sbg_partitioner {

unordered_map<SetPiece, Set, SetPieceHash> CommunicationCost::_communication_by_set_piece = {};

namespace internal {


// the only real instance
CommunicationCostPtr cost_matrix = nullptr;


namespace {

Set set_piece_communication(const SetPiece& nodes, const WeightedSBGraph& graph)
{
    // convert nodes into a set
    auto node_set = graph.fact().createSet(nodes);

    // compute preImage of map1 and map2 to get the edges that connects `nodes`
    auto edges_map1 = graph.map1().preImage(node_set);
    auto edges_map2 = graph.map2().preImage(node_set);

    // Now compute the disjoint union to remove loop edges
    auto communication = edges_map1.cup(edges_map2).difference(edges_map1.intersection(edges_map2));

    return communication;
}

}

}


CommunicationCost::CommunicationCost(const WeightedSBGraph& graph, PartitionMap partitions)
    : ICommunicationCost(),
    _graph(graph),
    _partitions(partitions)
{
    initialize();
}


void CommunicationCost::initialize()
{
    // compute cost by interval and partitions
    _cost_by_partition.reserve(_partitions.size());
    _ec_cost_by_interval.reserve(_partitions.size());
    _ic_cost_by_interval.reserve(_partitions.size());
    for (size_t i = 0; i < _partitions.size(); i++) {
        Set partition_i_communication = _graph.fact().createSet();
        Set internal_communication_partition_i = _graph.fact().createSet();

        for (const auto& node : _partitions.at(i)) {
            if (_communication_by_set_piece.find(node) == _communication_by_set_piece.end()) {
                _communication_by_set_piece.insert({node, internal::set_piece_communication(node, _graph)});
            }

            const auto& node_edges = _communication_by_set_piece.at(node);
            internal_communication_partition_i = node_edges.intersection(partition_i_communication).cup(internal_communication_partition_i);
            partition_i_communication = partition_i_communication.cup(node_edges);            
        }

        auto ec_parition_i = partition_i_communication.difference(internal_communication_partition_i);
        _cost_by_partition.emplace_back(make_pair(ec_parition_i, move(internal_communication_partition_i)));

        _ic_cost_by_interval.emplace_back();    // save space for this, will be filled on demand
        _ec_cost_by_interval.emplace_back();
    }
}


void CommunicationCost::update_partitions(PartitionMap& partitions, optional<reference_wrapper<const list<size_t>>> modified_partitions)
{
    _partitions = partitions;
    if (modified_partitions) {
        // now, update communication for partitions that were updated
        for (size_t i : modified_partitions->get()) {
            Set partition_i_communication = _graph.fact().createSet();
            Set internal_communication_partition_i = _graph.fact().createSet();

            for (const auto& node : _partitions.at(i)) {
                if (_communication_by_set_piece.find(node) == _communication_by_set_piece.end()) {
                    _communication_by_set_piece.insert({node, internal::set_piece_communication(node, _graph)});
                }

                const auto& node_edges = _communication_by_set_piece.at(node);
                internal_communication_partition_i = node_edges.intersection(partition_i_communication).cup(internal_communication_partition_i);
                partition_i_communication = partition_i_communication.cup(node_edges);
            }

            auto ec_parition_i = partition_i_communication.difference(internal_communication_partition_i);
            _cost_by_partition[i] = (make_pair(ec_parition_i, move(internal_communication_partition_i)));

            _ic_cost_by_interval[i].clear();
            _ec_cost_by_interval[i].clear();
        }
    } else {
        // if modified partitions was not provided, update everything
        _cost_by_partition.clear();
        _ec_cost_by_interval.clear();
        _ic_cost_by_interval.clear();
        initialize();
    }
}


Set CommunicationCost::get_ec_by_partition_id(unsigned partition_id)
{
    return _cost_by_partition[partition_id].first;
}


pair<Set, Set> CommunicationCost::compute_ec_ic(unsigned partition_id, const SetPiece& nodes)
{
    if (_communication_by_set_piece.find(nodes) == _communication_by_set_piece.end()) {
        _communication_by_set_piece.insert({nodes, internal::set_piece_communication(nodes, _graph)});
    }

    auto communication = _communication_by_set_piece.at(nodes);

    auto ec = communication.intersection(_cost_by_partition[partition_id].first);
    auto ic = communication.difference(ec);
    _ec_cost_by_interval[partition_id].insert({nodes, ec});
    _ic_cost_by_interval[partition_id].insert({nodes, ic});

    return { ec, ic };
}


Set CommunicationCost::get_ec_by_interval(unsigned partition_id, const SetPiece& nodes)
{
    if (_ec_cost_by_interval[partition_id].find(nodes) != _ec_cost_by_interval[partition_id].end()) {
        return _ec_cost_by_interval[partition_id].at(nodes);
    }

    if (_communication_by_set_piece.find(nodes) == _communication_by_set_piece.end()) {
        _communication_by_set_piece.insert({nodes, internal::set_piece_communication(nodes, _graph)});
    }

    auto [ec, _] = compute_ec_ic(partition_id, nodes);

    return ec;
}


Set CommunicationCost::get_ic_by_interval(unsigned partition_id, const SetPiece& nodes)
{
    if (_ic_cost_by_interval[partition_id].find(nodes) != _ic_cost_by_interval[partition_id].end()) {
        return _ic_cost_by_interval[partition_id].at(nodes);
    }

    if (_communication_by_set_piece.find(nodes) == _communication_by_set_piece.end()) {
        _communication_by_set_piece.insert({nodes, internal::set_piece_communication(nodes, _graph)});
    }

    auto [_, ic] = compute_ec_ic(partition_id, nodes);

    return ic;
}



CommunicationCostSync::CommunicationCostSync(const WeightedSBGraph& graph, PartitionMap partitions)
    :ICommunicationCost(),
    _comm_cost(graph, partitions)
{}


void CommunicationCostSync::update_partitions(PartitionMap& partitions, optional<reference_wrapper<const list<size_t>>> modified_partitions)
{
    const lock_guard<mutex> lock(_mutex);
    _comm_cost.update_partitions(partitions, modified_partitions);
}


Set CommunicationCostSync::get_ec_by_partition_id(unsigned partition_id)
{
    const lock_guard<mutex> lock(_mutex);
    return _comm_cost.get_ec_by_partition_id(partition_id);
}


Set CommunicationCostSync::get_ec_by_interval(unsigned partition_id, const SetPiece& nodes)
{
    const lock_guard<mutex> lock(_mutex);
    return _comm_cost.get_ec_by_interval(partition_id, nodes);
}


Set CommunicationCostSync::get_ic_by_interval(unsigned partition_id, const SetPiece& nodes)
{
    const lock_guard<mutex> lock(_mutex);
    return _comm_cost.get_ic_by_interval(partition_id, nodes);
}



CommunicationCostPtr create_communication_cost(const WeightedSBGraph& graph, PartitionMap partitions, bool multithreading_enabled)
{
    if (multithreading_enabled) {
        return make_unique<CommunicationCostSync>(graph, partitions);
    } else {
        return make_unique<CommunicationCost>(graph, partitions);
    }
}

void set_communication_cost(CommunicationCostPtr&& cost_matrix)
{
    internal::cost_matrix = move(cost_matrix);
}

ICommunicationCost& get_communication_cost()
{
    assert(internal::cost_matrix);
    return *internal::cost_matrix;
}

}