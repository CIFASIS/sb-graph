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

#include <filesystem>
#include <fstream>

#include <sbg/sbg.hpp>

#include "build_sb_graph.hpp"
#include "partition_metrics_api.hpp"
#include "weighted_sb_graph.hpp"

using namespace std;

using namespace SBG::LIB;
using namespace SBG::Util;

namespace sbg_partitioner {

namespace metrics {

namespace {

Set get_edge_cut(const Set& partition, const PWMap& maps_1, const PWMap& maps_2)
{
  auto d = maps_1.preImage(partition);
  auto im = maps_2.image(d);
  auto ec_nodes = im.difference(partition);
  auto external_communication = maps_2.preImage(ec_nodes);
  // avoid oversizing external communication
  external_communication = external_communication.intersection(d);

  return external_communication;
}

int communication_volume_one_dim(const PartitionMap& partitions, const SetPiece& v, const WeightedSBGraph& sb_graph, unsigned i)
{
  // avoid repeating this
  auto look_for_adjacents = [](const Set& nodes, const PWMap& map1, const PWMap& map2) {
    auto involved_edges1 = map1.preImage(nodes);
    auto arrival_nodes1 = map2.image(involved_edges1);
    arrival_nodes1 = arrival_nodes1.difference(nodes);

    auto involved_edges2 = map2.preImage(nodes);
    auto arrival_nodes2 = map1.image(involved_edges2);
    arrival_nodes2 = arrival_nodes2.difference(nodes);

    auto arrival_nodes = arrival_nodes1.cup(arrival_nodes2);

    return arrival_nodes;
  };

  int acc = 0;
  for (unsigned value = v.intervals()[0].begin(); value <= v.intervals()[0].end(); value += v.intervals()[0].step()) {
    int local_total_vol = 0;
    Interval set_piece = Interval(value, v.intervals()[0].step(), value);
    auto s = SET_FACT.createSet(set_piece);
    const Set adjacents = look_for_adjacents(s, sb_graph.map1(), sb_graph.map2());

    for (unsigned j = 0; j < partitions.size(); j++) {
      if (i == j) {
        continue;
      }

      const auto& p_set = partitions.at(j);

      if (not adjacents.intersection(from_vector(p_set)).isEmpty()) {
        local_total_vol += 1;
      }
    }

    acc += local_total_vol;
  }

  return acc;
}

void write_node_by_partition(const PartitionMap& partitions, const WeightedSBGraph& sb_graph)
{
  Partition nodes;
  nodes.reserve(sb_graph.V().size());
  for (auto v : sb_graph.V()) {
    nodes.push_back(v);
  }

  sort_partition_intervals(nodes);

  // expand it and write it
  vector<unsigned> partition_by_node;
  ofstream output_file("output.txt");
  for (const auto& n : nodes) {
    for (unsigned v_0 = n.intervals()[0].begin(); v_0 <= n.intervals()[0].end(); v_0++) {
      SetPiece set_piece;
      set_piece.emplaceBack(Interval(v_0, n.intervals()[0].step(), v_0));
      for (size_t i = 0; i < partitions.size(); i++) {
        Partition p = partitions.at(i);
        auto p_set = from_vector(p);
        if (not SET_FACT.createSet(set_piece).intersection(p_set).isEmpty()) {
          partition_by_node.push_back(i);
          output_file << to_string(i) << endl;
          break;
        }
      }
    }
  }
}

}  // namespace

int edge_cut(const PartitionMap& partitions, const WeightedSBGraph& sb_graph)
{
  cout << "computing edge cut " << endl;
  Set ec = SET_FACT.createSet();
  const auto& maps_1 = sb_graph.map1();
  const auto& maps_2 = sb_graph.map2();
  for (size_t i = 0; i < partitions.size(); i++) {
    cout << "partition " << i << endl;
    Set partition = from_vector(partitions.at(i));
    ec = ec.cup(get_edge_cut(partition, maps_1, maps_2));
    ec = ec.cup(get_edge_cut(partition, maps_2, maps_1));
  }

  int weight = get_edge_set_cost(ec, sb_graph.get_edge_costs());
  cout << "ec is " << weight << endl;

  return weight;
}

pair<int, int> communication_volume(const PartitionMap& partitions, const WeightedSBGraph& sb_graph)
{
  int comm_vol = 0;
  int max_comm_vol = 0;
  for (unsigned i = 0; i < partitions.size(); i++) {
    int communication_volume_partition = 0;
    const auto& p = partitions.at(i);

    for (const auto& v : p) {
      communication_volume_partition += communication_volume_one_dim(partitions, v, sb_graph, i);
    }

    comm_vol += communication_volume_partition;
    max_comm_vol = max(communication_volume_partition, max_comm_vol);
  }

  return {comm_vol, max_comm_vol};
}

float maximum_imbalance(const PartitionMap& partitions, const WeightedSBGraph& sb_graph)
{
  unsigned number_of_nodes = get_node_size(sb_graph.V(), sb_graph.get_node_weights());
  float expected_imb = number_of_nodes / partitions.size();

  float max_imbalance = 0.;
  for (const auto& p : partitions) {
    unsigned size_of_p = get_partition_size(p, sb_graph.get_node_weights());
    float imbalance_p = abs(expected_imb - float(size_of_p)) / expected_imb;
    max_imbalance = max(max_imbalance, imbalance_p);
  }

  write_node_by_partition(partitions, sb_graph);

  return max_imbalance;
}

PartitionMap read_partition_from_file(const string& name, const WeightedSBGraph& sb_graph, int number_of_partitions)
{
  ifstream file(name);
  string line;  // String to store each line of the file.

  PartitionMap partitions(number_of_partitions, Partition());
  if (file.is_open()) {
    // Read each line from the file and store it in the
    // 'line' variable.
    int node_counter = 0;
    while (getline(file, line)) {
      partitions[stoi(line)].push_back({Interval(node_counter, 1, node_counter)});
      node_counter++;
    }

    // Close the file stream once all lines have been
    // read.
    file.close();
  } else {
    // Print an error message to the standard error
    // stream if the file cannot be opened.
    cerr << "Unable to open file! " << name << endl;
  }

  return partitions;
}

ostream& operator<<(ostream& os, const communication_metrics& comm_metrics)
{
  os << "communication metrics, edge cut: " << comm_metrics.edge_cut << ", communication volume: " << comm_metrics.comm_volume
     << ", maximum communication volume: " << comm_metrics.max_comm_volume << ", maximum imbalance: " << comm_metrics.maximum_imbalance;

  return os;
}

void dump_results(const SBG::LIB::WeightedSBGraph& sb_graph, const std::string& filename, const PartitionMap& partition)
{
  cout << "Dump results!" << endl;
  ofstream edges_file(std::filesystem::path(filename).stem().string() + "_edges.txt");
  auto edges = sb_graph.E().compact();
  if (edges.arity() == 2) {
    size_t row_size = (*sb_graph.V().begin()).intervals()[0].cardinal();
    cout << "row size " << row_size << endl;
    for (auto it = edges.begin(); it != edges.end(); ++it) {
      auto departure = sb_graph.map1().image(SET_FACT.createSet(*it));
      auto arrival = sb_graph.map2().image(SET_FACT.createSet(*it));
      cout << "departure " << departure << " arrival " << arrival << endl;
      if (departure.size() > 1 or arrival.size() > 1) {
        cout << "we are not ready for this" << endl;
        throw 1;
      }

      // outer iterators
      auto dep_it = departure.begin();
      auto arrival_it = arrival.begin();

      for (; dep_it != departure.end(), arrival_it != arrival.end(); ++dep_it, ++arrival_it) {
        // set pieces
        auto set_piece_dep = *dep_it;
        auto set_piece_arr = *arrival_it;
        for (size_t dep_row_val = set_piece_dep.intervals()[0].begin(), arr_row_val = set_piece_arr.intervals()[0].begin();
             dep_row_val <= set_piece_dep.intervals()[0].end() and arr_row_val <= set_piece_arr.intervals()[0].end();
             dep_row_val++, arr_row_val++) {
          for (size_t dep_col_val = set_piece_dep.intervals()[1].begin(), arr_col_val = set_piece_arr.intervals()[1].begin();
               dep_col_val <= set_piece_dep.intervals()[1].end() and arr_col_val <= set_piece_arr.intervals()[1].end();
               dep_col_val++, arr_col_val++) {
            size_t dep_val = dep_row_val * row_size + dep_col_val;
            size_t arr_val = arr_row_val * row_size + arr_col_val;
            edges_file << dep_val << " " << arr_val << endl;
            cout << dep_val << ", " << arr_val << ", " << sb_graph.V().cardinal() << endl;
            assert(sb_graph.V().cardinal() > dep_val);
            assert(sb_graph.V().cardinal() > arr_val);
          }
        }
      }
    }
  } else {
    for (size_t i = (*edges.begin())[0].begin(); i <= (*edges.begin())[0].end(); i++) {
      auto departure = (*sb_graph.map1().image(SBG::LIB::SET_FACT.createSet(SBG::LIB::Interval(i))).begin())[0].begin();
      auto arrival = (*sb_graph.map2().image(SBG::LIB::SET_FACT.createSet(SBG::LIB::Interval(i))).begin())[0].begin();
      edges_file << departure << " " << arrival << endl;
    }
  }
  cout << "connections ok" << endl;

  vector<unsigned> partition_vector(sb_graph.V().cardinal(), 0);
  if (sb_graph.V().arity() == 2) {
    size_t row_size = (*sb_graph.V().begin()).intervals()[0].cardinal();
    for (unsigned i = 0; i < partition.size(); i++) {
      for (const auto& n : partition.at(i)) {
        for (size_t v_0 = n.intervals()[0].begin(); v_0 <= n.intervals()[0].end(); v_0++) {
          for (size_t v_1 = n.intervals()[1].begin(); v_1 <= n.intervals()[1].end(); v_1++) {
            size_t vertex = v_0 * row_size + v_1;
            partition_vector[vertex] = i;
          }
        }
      }
    }
  } else {
    for (unsigned i = 0; i < partition.size(); i++) {
      for (size_t vert_idx = 0; vert_idx < partition.at(i).size(); vert_idx++) {
        for (size_t val = (*partition.at(i).at(vert_idx).begin()).begin(); val <= (*partition.at(i).at(vert_idx).begin()).end(); val++) {
          partition_vector[val] = i;
        }
      }
    }
  }
  cout << "parts ok" << endl;

  ofstream parts_file(std::filesystem::path(filename).stem().string() + "_parts.txt");
  for_each(partition_vector.begin(), partition_vector.end(), [&parts_file](const auto val) { parts_file << val << "\n"; });
}

}  // namespace metrics

}  // namespace sbg_partitioner
