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
#include "dfs_on_sbg.hpp"
#include "partition_graph.hpp"
#include "sbg_partitioner_log.hpp"

#define TRY_MULTIPLE_STRATEGIES 1

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

[[maybe_unused]] size_t get_partition_communication(WeightedSBGraph& graph, const PartitionMap& partitions, SetAF& set_fact)
{
  Set s = set_fact.createSet();
  for (size_t i = 0; i < partitions.size(); i++) {
    auto ss = get_connectivity_set(graph, partitions, i, set_fact);
    s = ss.cup(s);
    // logging::sbg_log << "current connectivity set " << s << ", cardinality " << get_OrdSet_size(s) << ", partition " << i << endl;
  }

  size_t size = get_OrdSet_size(s);

  return size;
}

constexpr bool using_many_initial_partitions = TRY_MULTIPLE_STRATEGIES;
}  // namespace


// we could cache solutions here
Set to_vector(const Partition& partition, SetAF& set_fact) {
    Set partition_set = set_fact.createSet();
    for (size_t i = 0; i < partition.size(); i++) {
        partition_set.emplace(partition[i]);
    }

    return partition_set;
}


vector<PartitionMap> make_initial_partitions(SBG::LIB::WeightedSBGraph& graph, unsigned number_of_partitions, SetAF& set_fact)
{
  vector<PartitionMap> partitions_sets;
  initialize_partitioning(graph, number_of_partitions, set_fact);

  constexpr bool pre_order = true;
  auto s1 = PartitionStrategyDistributive(number_of_partitions, graph, set_fact);
  add_strategy(s1, pre_order);
#if TRY_MULTIPLE_STRATEGIES
  auto s2 = PartitionStrategyDistributive(number_of_partitions, graph, set_fact);
  add_strategy(s2, not pre_order);
  auto s3 = PartitionStrategyGreedy(number_of_partitions, graph, set_fact);
  add_strategy(s3, pre_order);
  auto s4 = PartitionStrategyGreedy(number_of_partitions, graph, set_fact);
  add_strategy(s4, not pre_order);
#endif

  vector<map<unsigned, set<SetPiece>>> partitions = partitionate();

  for (const auto& partition : partitions) {
    PartitionMap partition_set;
    for (const auto& [id, set] : partition) {
      Set one_partition_set = set_fact.createSet();
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
    sanity_check(graph, p, number_of_partitions);
  });

  return partitions_sets;
}

PartitionMap best_initial_partition(WeightedSBGraph& graph, unsigned number_of_partitions, SetAF& set_fact)
{
  std::vector<sbg_partitioner::PartitionMap> partition_maps = make_initial_partitions(graph, number_of_partitions, set_fact);

  auto& best_initial_partitions = partition_maps.front();
  if (using_many_initial_partitions) {
    size_t best_communication_set_cardinality = get_partition_communication(graph, best_initial_partitions, set_fact);

    for (size_t i = 1; i < partition_maps.size(); i++) {
      auto temp_intial_partitions = partition_maps[i];
      size_t temp_partition_comm_size = get_partition_communication(graph, temp_intial_partitions, set_fact);

      if (temp_partition_comm_size < best_communication_set_cardinality) {
        best_initial_partitions = std::move(temp_intial_partitions);
        best_communication_set_cardinality = temp_partition_comm_size;
      }
    }

    logging::sbg_log << "Best is " << best_initial_partitions << " with communication " << best_communication_set_cardinality << endl;
  }

  return best_initial_partitions;
}

Set get_connectivity_set(SBG::LIB::SBG& graph, const PartitionMap& partitions, size_t partition_index, SetAF& set_fact)
{
    const auto& partition_vector = partitions.at(partition_index);
    Set partition = set_fact.createSet();
    for_each(partition_vector.cbegin(), partition_vector.cend(), [&partition] (auto s) { partition.emplaceBack(s); });

    auto comm_edges_1 = get_communication_edges(partition, graph.map1(), graph.map2());
    auto comm_edges_2 = get_communication_edges(partition, graph.map2(), graph.map1());
    auto comm_edges = comm_edges_1.cup(comm_edges_2);

    return comm_edges;
}

size_t get_OrdSet_size(const Set& set)
{
  size_t acc = 0;
  for (auto it = set.begin(); it != set.end(); ++it) {
    const auto& set_piece = *it;
    for (auto& interval : set_piece.intervals()) {
      acc += (interval.end() - interval.begin() + 1);
    }
  }

  return acc;
}

void sanity_check(const WeightedSBGraph& graph, PartitionMap& partitions_set, unsigned number_of_partitions)
{
#ifdef PARTITION_SANITY_CHECK
  // This is just a sanity check
  OrdSet nodes_to_check;
  for (unsigned i = 0; i < number_of_partitions; i++) {
    nodes_to_check = cup(nodes_to_check, partitions_set[i]);
  }
  OrdSet diff_1 = difference(graph.V(), nodes_to_check);
  OrdSet diff_2 = difference(nodes_to_check, graph.V());
  assert(get_node_size(diff_1, graph.get_node_weights()) == 0 and "The intial partition has less elements than the graph");
  assert(get_node_size(diff_2, graph.get_node_weights()) == 0 and "The intial partition has more elements than the graph");
  for (unsigned i = 0; i < number_of_partitions; i++) {
    for (unsigned j = i + 1; j < number_of_partitions; j++) {
      auto p_1 = partitions_set[i];
      auto p_2 = partitions_set[j];
      stringstream error_msg;
      error_msg << "Intersection between " << i << " and " << j << " is not empty." << endl;
      assert(intersection(p_1, p_2).pieces().empty() and error_msg.str().c_str());
    }
  }
#endif  // PARTITION_SANITY_CHECK
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


ostream& operator<<(ostream& os, const Partition& partition)
{
    for_each(partition.cbegin(), partition.cend(), [&os] (const auto& p) { os << p << " "; });

    return os;
}


ostream& operator<<(ostream& os, const PartitionMap& partitions)
{
  for (size_t i = 0; i < partitions.size(); i++) {
    os << i << " " << partitions[i] << " ";
  }

  return os;
}

}  // namespace sbg_partitioner