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

#include <algorithms/cc/cc.hpp>
#include <sbg/sbg.hpp>
#include <unordered_map>

#include "communication_cost.hpp"
#include "partition_graph.hpp"
#include "partition_graph_cc.hpp"
#include "sbg_partitioner_log.hpp"


using namespace std;

using namespace SBG::LIB;

namespace sbg_partitioner {

std::size_t SetPieceHash::set_piece_hash(const SetPiece& set_piece) {
  constexpr size_t magic_number = 0x9e3779b9;
  std::size_t seed = 0;
  for (const auto& interval : set_piece) {
    seed ^= std::hash<int>()(interval.begin()) + magic_number + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>()(interval.end()) + magic_number + (seed << 6) + (seed >> 2);
  }

  return seed;
}

std::size_t SetPieceHash::operator()(const SetPiece& set_piece) const {
  std::size_t seed = set_piece_hash(set_piece);

  return seed;
}


std::size_t SetHash::operator()(const SBG::LIB::Set& set) const
{
    std::size_t seed = set.size();
    for (const auto& set_piece : set) {
        seed ^= SetPieceHash::set_piece_hash(set_piece) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}


unordered_map<SetPiece, Set, SetPieceHash> CommunicationCost::_communication_by_set_piece = {};

namespace internal {


// the only real instance
CommunicationCostPtr cost_matrix = nullptr;


namespace {


Set set_piece_communication(const SetPiece& nodes, const WeightedSBGraph& graph)
{
    // convert nodes into a set
    auto node_set = SET_FACT.createSet(nodes);

    // compute preImage of map1 and map2 to get the edges that connects `nodes`
    auto edges_map1 = graph.map1().preImage(node_set);
    auto edges_map2 = graph.map2().preImage(node_set);

    // Now compute the disjoint union to remove loop edges
    auto common_edges = edges_map1.intersection(edges_map2);
    auto communication = edges_map1.cup(edges_map2).difference(common_edges);

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
        Set partition_i_communication = SET_FACT.createSet();
        Set internal_communication_partition_i = SET_FACT.createSet();

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
            Set partition_i_communication = SET_FACT.createSet();
            Set internal_communication_partition_i = SET_FACT.createSet();

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


Set CommunicationCost::get_set_piece_edges(const SBG::LIB::SetPiece& nodes)
{
    if (_communication_by_set_piece.find(nodes) == _communication_by_set_piece.end()) {
        _communication_by_set_piece.insert({nodes, internal::set_piece_communication(nodes, _graph)});
    }

    return _communication_by_set_piece.at(nodes);
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


Set CommunicationCostSync::get_set_piece_edges(const SetPiece& nodes)
{
    const lock_guard<mutex> lock(_mutex);
    return _comm_cost.get_set_piece_edges(nodes);
}



CommunicationCostCC::CommunicationCostCC(const WeightedSBGraph& graph, const using_cc::SetPointers& nodes)
    : _graph(graph),
    _sorted_nodes(nodes),
    _set_piece_indices(PW_FACT.createPWMap())
{
    initialize();
}


void CommunicationCostCC::initialize()
{
    _adjacency_matrix = vector<vector<unsigned> >(_sorted_nodes.size(), vector<unsigned>(_sorted_nodes.size(), 0));

    for (size_t i = 0; i <  _sorted_nodes.size(); i++) {
        const auto& set_piece_pointer = _sorted_nodes.at(i);
        auto edges = get_set_piece_edges(set_piece_pointer.set_piece);

        for (size_t j = i + 1; j < _sorted_nodes.size(); j++) {
            const auto& set_piece_pointer_j = _sorted_nodes.at(j);
            auto edges2 = get_set_piece_edges(set_piece_pointer_j.set_piece);
            auto shared_edges_cardinal = edges.intersection(edges2).cardinal();
            if (not edges.intersection(edges2).isEmpty()) {
                logging::sbg_log << set_piece_pointer.set_piece << ", " << set_piece_pointer_j.set_piece << ": " << shared_edges_cardinal << endl;
                _adjacency_matrix[set_piece_pointer.index][set_piece_pointer_j.index] = shared_edges_cardinal;
                _adjacency_matrix[set_piece_pointer_j.index][set_piece_pointer.index] = shared_edges_cardinal;
            }
        }
    }

#ifdef SBG_PARTITIONER_LOGGING
    for (size_t i = 0; i <  _sorted_nodes.size(); i++) {
        for (size_t j = 0; j < _sorted_nodes.size(); j++) {
            cout << unsigned(_adjacency_matrix[_sorted_nodes.at(i).index][_sorted_nodes.at(j).index]) << " ";
        }
        cout << endl;
    }
#endif
}


unsigned CommunicationCostCC::get_communication(const SBG::LIB::SetPiece& a, const SBG::LIB::SetPiece& b) const
{
    auto a_set = SET_FACT.createSet(a);
    auto b_set = SET_FACT.createSet(b);

    unsigned a_idx = (*_set_piece_indices.image(a_set).begin())[0].begin();
    unsigned b_idx = (*_set_piece_indices.image(b_set).begin())[0].begin();

    unsigned cost = _adjacency_matrix[a_idx][b_idx];

    return cost;
}


unsigned CommunicationCostCC::get_communication(const SBG::LIB::SetPiece& a) const
{
    auto a_set = SET_FACT.createSet(a);
    unsigned a_idx = (*_set_piece_indices.image(a_set).begin())[0].begin();
    unsigned cost = 0;
    for_each(_adjacency_matrix[a_idx].cbegin(), _adjacency_matrix[a_idx].cend(), [&cost](unsigned c) { cost += c; });

    return cost;
}


unsigned CommunicationCostCC::get_communication(unsigned a_idx, unsigned b_idx) const
{
    return _adjacency_matrix[a_idx][b_idx];
}


unsigned CommunicationCostCC::get_communication(unsigned idx) const
{
    const auto& row = _adjacency_matrix[idx];

    return std::accumulate(row.begin(), row.end(), 0U); // 0U ensures sum starts as an unsigned
}


Set CommunicationCostCC::get_set_piece_edges(const SBG::LIB::SetPiece& nodes)
{
    if (_communication_by_set_piece.find(nodes) == _communication_by_set_piece.end()) {
        _communication_by_set_piece.insert({nodes, internal::set_piece_communication(nodes, _graph)});
    }

    return _communication_by_set_piece.at(nodes);
}


unsigned CommunicationCostCC::get_index(const SBG::LIB::SetPiece& nodes) const
{
    return (*_set_piece_indices.image(SET_FACT.createSet(nodes)).begin())[0].begin();
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
