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

 #include "cost.hpp"


using namespace std;

using namespace SBG::LIB;

namespace sbg_partitioner {

namespace internal {


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
    const PWMap& map_1,
    const PWMap& map_2,
    const SetAF& set_fact)
{
    ec_ic cost1 = compute_EC_IC_from_map_1_to_map_2(partition, nodes, map_1, map_2, set_fact);
    ec_ic cost2 = compute_EC_IC_from_map_1_to_map_2(partition, nodes, map_2, map_1, set_fact);

    ec_ic cost = ec_ic(cost1.first.cup(cost2.first), cost1.second.cup(cost2.second));

    return cost;
}

}

CostMatrix::CostMatrix(const WeightedSBGraph& graph, PartitionMap& partitions, const SetAF& set_fact)
    : _graph(graph),
    _partitions(partitions),
    _set_fact(set_fact)
{
    initialize();
}

void CostMatrix::initialize()
{
    // compute cost by interval and partitions
    _cost_by_partition.reserve(_partitions.size());
    for (size_t i = 0; i < _partitions.size(); i++) {

        _cost_by_partition.emplace_back(make_pair(_set_fact.createSet(), _set_fact.createSet()));
        for (const auto& node : _partitions.at(i)) {
            auto [ec, ic] = internal::compute_EC_IC(_partitions.at(i), node, _graph.map1(), _graph.map2(), _set_fact);

            _cost_by_partition.back() = {  _cost_by_partition.back().first.cup(ec), _cost_by_partition.back().second.cup(ic) };
            _ec_cost_by_interval.insert({node, ec});
            _ic_cost_by_interval.insert({node, ic});
        }
    }
}

void CostMatrix::update_partition(unsigned i, const Partition& partition)
{
    _partitions[i] = partition;

}

void CostMatrix::update_partitions(PartitionMap& partitions)
{
    _partitions = partitions;
    _cost_by_partition.clear();
    _ec_cost_by_interval.clear();
    _ic_cost_by_interval.clear();
    initialize();
}

Set CostMatrix::get_ec_by_partition_id(unsigned partition_id)
{
    return _cost_by_partition[partition_id].first;
}

Set CostMatrix::get_ec_by_interval(unsigned partition_id, const SetPiece& nodes)
{
    if (_ec_cost_by_interval.find(nodes) != _ec_cost_by_interval.end()) {
        return _ec_cost_by_interval.at(nodes);
    }

    auto cost = internal::compute_EC_IC(_partitions.at(partition_id), nodes, _graph.map1(), _graph.map2(), _set_fact);
    _ec_cost_by_interval.insert({nodes, cost.first});
    _ic_cost_by_interval.insert({nodes, cost.second});

    return cost.first;
}

Set CostMatrix::get_ic_by_interval(unsigned partition_id, const SetPiece& nodes)
{
    if (_ic_cost_by_interval.find(nodes) != _ic_cost_by_interval.end()) {
        return _ic_cost_by_interval.at(nodes);
    }

    auto cost = internal::compute_EC_IC(_partitions.at(partition_id), nodes, _graph.map1(), _graph.map2(), _set_fact);
    _ec_cost_by_interval.insert({nodes, cost.first});
    _ic_cost_by_interval.insert({nodes, cost.second});

    return cost.second;
}


}