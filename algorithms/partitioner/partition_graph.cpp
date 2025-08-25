/*****************************************************************************

 This file is part of SBG Partitioner.

 SBG Partitioner is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Partitioner is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Partitioner.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <set>
#include <util/logger.hpp>

#include "build_sb_graph.hpp"
#include "communication_cost.hpp"
#include "dfs_on_sbg.hpp"
#include "partition_graph.hpp"
#include "sbg_partitioner_log.hpp"


using namespace std;

using namespace SBG::LIB;
using namespace SBG::Util;

using namespace sbg_partitioner::search;

namespace sbg_partitioner {

// Using an unnamed manespace to define functions with internal linkage
namespace {

Set get_communication_edges(Set partition, const PWMap& map_1, const PWMap& map_2)
{
  auto pre_image = map_1.preImage(partition);
  auto image_map_2 = map_2.image(pre_image);
  auto outside_partition = image_map_2.difference(partition);
  auto comm_edges = map_2.preImage(outside_partition);

  return comm_edges;
}


[[maybe_unused]] size_t get_partition_communication(WeightedSBGraph& graph, const PartitionMap& partitions)
{
  Set s = graph.fact().createSet();
  for (size_t i = 0; i < partitions.size(); i++) {
    auto ss = get_connectivity_set(graph, partitions, i);
    s = ss.cup(s);
  }

  size_t size = get_set_size(s);

  return size;
}


vector<PartitionMap> make_initial_partitions(SBG::LIB::WeightedSBGraph& graph, unsigned number_of_partitions,
    const InitialPartitionStrategy strategy)
{
  vector<PartitionMap> partitions_sets;
  initialize_partitioning(graph, number_of_partitions);

  constexpr bool pre_order = true;
  const bool all_strategies = strategy == InitialPartitionStrategy::ALL;
  if (all_strategies or strategy == InitialPartitionStrategy::DFS_DISTRIBUTIVE_PREORDER) {
    auto s1 = make_unique<PartitionStrategyDistributive>(number_of_partitions, graph);
    add_strategy(move(s1), pre_order);
  }

  if (all_strategies or strategy == InitialPartitionStrategy::DFS_DISTRIBUTIVE_POSTORDER) {
    auto s2 = make_unique<PartitionStrategyDistributive>(number_of_partitions, graph);
    add_strategy(move(s2), not pre_order);
  }

  if (all_strategies or strategy == InitialPartitionStrategy::DFS_GREEDY_PREORDER) {
    auto s3 = make_unique<PartitionStrategyGreedy>(number_of_partitions, graph);
    add_strategy(move(s3), pre_order);
  }

  if (all_strategies or strategy == InitialPartitionStrategy::DFS_GREEDY_POSTORDER) {
    auto s4 = make_unique<PartitionStrategyGreedy>(number_of_partitions, graph);
    add_strategy(move(s4), not pre_order);
  }

  vector<map<unsigned, set<SetPiece>>> partitions = partitionate();

  for (const auto& partition : partitions) {
    PartitionMap partition_set;
    for (const auto& [id, set] : partition) {
      Set one_partition_set = graph.fact().createSet();
      Partition p;
      for (auto& s : set) {
        SetPiece intervals;
        if (not s.intervals().empty()) {
          for (size_t i = 0; i < s.intervals().size(); i++) {
            Interval interv = s.intervals()[i];
            intervals.emplaceBack(interv);
          }
        }
        p.emplace_back(intervals);
      }
      partition_set.push_back(p);
    }

    partitions_sets.push_back(move(partition_set));
  }

  for_each(partitions_sets.begin(), partitions_sets.end(), [&graph, number_of_partitions](PartitionMap& p) {
    logging::sbg_log << p << endl;
    if (sanity_check_enabled) {
      sanity_check(graph, p, number_of_partitions);
    }
  });

  return partitions_sets;
}

}  // namespace


// we could cache solutions here
Set from_vector(const Partition& partition, const SetAF& set_fact) {
    Set partition_set = set_fact.createSet();
    for (size_t i = 0; i < partition.size(); i++) {
        partition_set.emplace(partition[i]);
    }

    return partition_set;
}


Partition to_vector(const Set& partition_set)
{
    Partition partition;
    for (auto set_piece : partition_set) {
        partition.push_back(move(set_piece));
    }

    return partition;
}


PartitionMap best_initial_partition(WeightedSBGraph& graph, unsigned number_of_partitions,
    const InitialPartitionStrategy strategy)
{
  logging::sbg_log << "computing strategy number " << strategy << endl;
  std::vector<sbg_partitioner::PartitionMap> partition_maps = make_initial_partitions(graph, number_of_partitions, strategy);

  auto& best_initial_partitions = partition_maps.front();
  unique_ptr<CommunicationCost> comm_cost = make_unique<CommunicationCost>(graph, best_initial_partitions);
  if (strategy == InitialPartitionStrategy::ALL) {

    auto best_communication_set = graph.fact().createSet();
    for (unsigned i = 0; i < number_of_partitions; i++) {
        best_communication_set = best_communication_set.cup(comm_cost->get_ec_by_partition_id(i));
    }
    size_t best_communication_set_size = best_communication_set.cardinal();

    for (size_t i = 1; i < partition_maps.size(); i++) {
        auto temp_intial_partitions = partition_maps[i];
        unique_ptr<CommunicationCost> temp_comm_cost = make_unique<CommunicationCost>(graph, temp_intial_partitions);

        auto temp_partition_comm = graph.fact().createSet();
        for (unsigned i = 0; i < number_of_partitions; i++) {
            temp_partition_comm = temp_partition_comm.cup(comm_cost->get_ec_by_partition_id(i));
        }

        size_t temp_intial_partitions_size = temp_partition_comm.cardinal();

        if (temp_intial_partitions_size < best_communication_set_size) {
          comm_cost = move(temp_comm_cost);
          best_initial_partitions = move(partition_maps[i]);
        }
    }

    logging::sbg_log << "Best is " << best_initial_partitions << " with communication " << best_communication_set << endl;
  }

  set_communication_cost(move(comm_cost));

  return best_initial_partitions;
}

Set get_connectivity_set(SBG::LIB::SBG& graph, const PartitionMap& partitions, size_t partition_index)
{
    const auto& partition_vector = partitions.at(partition_index);
    Set partition = graph.fact().createSet();
    for_each(partition_vector.cbegin(), partition_vector.cend(), [&partition] (auto s) { partition.emplaceBack(s); });

    auto comm_edges_1 = get_communication_edges(partition, graph.map1(), graph.map2());
    auto comm_edges_2 = get_communication_edges(partition, graph.map2(), graph.map1());
    auto comm_edges = comm_edges_1.cup(comm_edges_2);

    return comm_edges;
}


void sanity_check(const WeightedSBGraph& graph, PartitionMap& partitions_set, unsigned number_of_partitions)
{
  // This is just a sanity check
  Set nodes_to_check = graph.fact().createSet();
  for (unsigned i = 0; i < number_of_partitions; i++) {
    nodes_to_check = nodes_to_check.cup(from_vector(partitions_set[i], graph.fact()));
  }

  Set diff = nodes_to_check.difference(graph.V());
  assert(get_node_size(diff, graph.get_node_weights(), graph.fact()) == 0 and "The intial partition has less elements than the graph");

  for (unsigned i = 0; i < number_of_partitions; i++) {
    for (unsigned j = i + 1; j < number_of_partitions; j++) {
      auto p_1 = from_vector(partitions_set[i], graph.fact());
      auto p_2 = from_vector(partitions_set[j], graph.fact());
      stringstream error_msg;
      error_msg << "Intersection between " << i << " and " << j << " is not empty." << endl;
      assert(p_1.intersection(p_2).isEmpty() and error_msg.str().c_str());
    }
  }
}

string get_output(const PartitionMap& partition_map)
{
  rapidjson::Document json_doc;
  rapidjson::Document::AllocatorType& allocator = json_doc.GetAllocator();
  json_doc.SetObject();

  rapidjson::Value obj_partitions(rapidjson::kArrayType);
  for (size_t i = 0; i < partition_map.size(); i++) {
    rapidjson::Value obj_partition(rapidjson::kArrayType);
    const auto& partition = partition_map.at(i);
    for (auto it = partition.begin(); it != partition.end(); ++it) {
      const SetPiece& set_piece = *it;
      rapidjson::Value obj_intervals(rapidjson::kArrayType);
      obj_intervals.SetArray();
      for (const Interval& interval : set_piece.intervals()) {
        rapidjson::Value obj_interval(rapidjson::kArrayType);
        rapidjson::Value begin(rapidjson::kNumberType);
        begin.SetUint(interval.begin());
        obj_interval.PushBack(begin, allocator);
        rapidjson::Value end(rapidjson::kNumberType);
        end.SetUint(interval.end());
        obj_interval.PushBack(end, allocator);

        obj_intervals.PushBack(obj_interval, allocator);
      }

      obj_partition.PushBack(obj_intervals, allocator);
    }

    rapidjson::Value obj_nodes(rapidjson::kObjectType);
    obj_nodes.AddMember("nodes", obj_partition, allocator);

    obj_partitions.PushBack(obj_nodes, allocator);
  }

  json_doc.AddMember("partitions", obj_partitions, allocator);

  // Write the JSON data to the file
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  json_doc.Accept(writer);
  string json_data = string(s.GetString());

  return json_data;
}


void sort_partition_intervals(Partition& p)
{
    constexpr auto compare_intervals = [](const SBG::LIB::SetPiece& s1, const SBG::LIB::SetPiece& s2) {
        return s1[0].begin() < s2[0].begin();
    };

    sort(p.begin(), p.end(), compare_intervals);
}


ostream& operator<<(ostream& os, const Partition& partition)
{
    for_each(partition.cbegin(), partition.cend(), [&os] (const auto& p) { os << p << " "; });

    return os;
}


ostream& operator<<(ostream& os, const PartitionMap& partitions)
{
  for (size_t i = 0; i < partitions.size(); i++) {
    os << i << " " << partitions[i] << endl;
  }

  return os;
}

}  // namespace sbg_partitioner