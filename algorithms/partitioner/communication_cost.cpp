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

namespace internal {


static CommunicationCost* cost_matrix = nullptr;


namespace {

ec_ic compute_EC_IC_from_map_1_to_map_2(
    const Partition& partition,
    const SetPiece& nodes,
    const PWMap& map_1,
    const PWMap& map_2,
    const SetAF& set_fact)
{
    auto nodes_set = set_fact.createSet(nodes);
    auto d = map_1.preImage(nodes_set);
    auto im = map_2.image(d);
    auto partition_set = from_vector(partition, set_fact);
    auto ic_nodes = partition_set.intersection(im);
    ic_nodes = ic_nodes.difference(nodes_set);
    auto ec_nodes = im.difference(ic_nodes);
    auto ic = map_2.preImage(ic_nodes).intersection(d);
    auto ec = map_2.preImage(ec_nodes).intersection(d);

    return make_pair(ec, ic);
}


}


ec_ic compute_EC_IC(
    const Partition& partition,
    const SetPiece& nodes,
    const SBG::LIB::WeightedSBGraph& graph)
{
    ec_ic cost1 = compute_EC_IC_from_map_1_to_map_2(partition, nodes, graph.map1(), graph.map2(), graph.fact());
    ec_ic cost2 = compute_EC_IC_from_map_1_to_map_2(partition, nodes, graph.map2(), graph.map1(), graph.fact());

    ec_ic cost = ec_ic(cost1.first.cup(cost2.first), cost1.second.cup(cost2.second));

    return cost;
}

}


CommunicationCost::CommunicationCost(const WeightedSBGraph& graph, PartitionMap partitions)
    : _graph(graph),
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

        _cost_by_partition.emplace_back(make_pair(_graph.fact().createSet(), _graph.fact().createSet()));
        _ec_cost_by_interval.emplace_back();
        _ic_cost_by_interval.emplace_back();
        for (const auto& node : _partitions.at(i)) {
            auto [ec, ic] = internal::compute_EC_IC(_partitions.at(i), node, _graph);

            _cost_by_partition.back() = {  _cost_by_partition.back().first.cup(ec), _cost_by_partition.back().second.cup(ic) };
            _ec_cost_by_interval.back().insert({node, ec});
            _ic_cost_by_interval.back().insert({node, ic});
        }
    }
}


void CommunicationCost::update_partitions(PartitionMap& partitions, optional<vector<size_t>> modified_partitions)
{
    _partitions = partitions;
    if (modified_partitions) {
        Set update_nodes = _graph.fact().createSet();
        // now, update communication for partitions that were updated
        for (size_t i : *modified_partitions) {
            _ec_cost_by_interval[i].clear();
            _ic_cost_by_interval[i].clear();
            update_nodes = update_nodes.cup(from_vector(_partitions.at(i), _graph.fact()));
            _cost_by_partition[i] = make_pair(_graph.fact().createSet(), _graph.fact().createSet());
            for (const auto& node : _partitions.at(i)) {
                auto [ec, ic] = internal::compute_EC_IC(_partitions.at(i), node, _graph);

                _cost_by_partition[i] = {  _cost_by_partition.at(i).first.cup(ec), _cost_by_partition.at(i).second.cup(ic) };
                _ec_cost_by_interval[i].insert_or_assign(node, ec);
                _ic_cost_by_interval[i].insert_or_assign(node, ic);
            }
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


Set CommunicationCost::get_ec_by_interval(unsigned partition_id, const SetPiece& nodes)
{
    if (_ec_cost_by_interval[partition_id].find(nodes) != _ec_cost_by_interval[partition_id].end()) {
        return _ec_cost_by_interval[partition_id].at(nodes);
    }

    auto cost = internal::compute_EC_IC(_partitions.at(partition_id), nodes, _graph);
    _ec_cost_by_interval[partition_id].insert({nodes, cost.first});
    _ic_cost_by_interval[partition_id].insert({nodes, cost.second});

    return cost.first;
}


Set CommunicationCost::get_ic_by_interval(unsigned partition_id, const SetPiece& nodes)
{
    if (_ic_cost_by_interval[partition_id].find(nodes) != _ic_cost_by_interval[partition_id].end()) {
        return _ic_cost_by_interval[partition_id].at(nodes);
    }

    auto cost = internal::compute_EC_IC(_partitions.at(partition_id), nodes, _graph);
    _ec_cost_by_interval[partition_id].insert({nodes, cost.first});
    _ic_cost_by_interval[partition_id].insert({nodes, cost.second});

    return cost.second;
}


void set_communication_cost(CommunicationCost& cost_matrix)
{
    internal::cost_matrix = new CommunicationCost(cost_matrix);
}

CommunicationCost& get_communication_cost()
{
    assert(internal::cost_matrix);
    return *internal::cost_matrix;
}

}