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

#include <future>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <unordered_map>

#include <util/time_profiler.hpp>

#include "build_sb_graph.hpp"
#include "communication_cost.hpp"
#include "kernighan_lin_partitioner.hpp"
#include "sbg_partitioner_log.hpp"

#define PARTITION_IMBALANCE_DEBUG 0
#define TEST_EDGE_CUT 0

// This code is based on https://github.com/CIFASIS/sbg-partitioner/discussions/17

using namespace std;

using namespace SBG::LIB;
using namespace SBG::Util;

namespace sbg_partitioner {

using ec_ic = std::pair<Set, Set>;

// Using unnamed namespace to define functions with internal linkage
namespace {

pair<unsigned, unsigned> compute_lmin_lmax(const WeightedSBGraph& graph, unsigned number_of_partitions, const float imbalance_epsilon)
{
  unsigned w_v = get_node_size(graph.V(), graph.get_node_weights());
  unsigned B = ceil(w_v / number_of_partitions);
  int im = imbalance_epsilon * B;
  unsigned LMin = B - im;
  unsigned LMax = B + im;

  return make_pair(LMin, LMax);
}

GainObjectImbalance generate_gain_object(int a_idx, unsigned partition_a_id, Partition partition_a, int b_idx, unsigned partition_b_id,
                                         Partition partition_b)
{
  auto& comm = get_communication_cost();
  auto set_a = partition_a.at(a_idx);
  auto set_b = partition_b.at(b_idx);
  if (set_a.cardinal() != set_b.cardinal()) {
    unsigned size = min(set_b.cardinal(), set_a.cardinal());
    set_a = cut_interval(set_a, set_a.begin()->begin() + size - 1).first;
    set_b = cut_interval(set_b, set_b.begin()->begin() + size - 1).first;
  }

  auto ic_a = comm.get_ic_by_interval(partition_a_id, set_a);
  auto ec_a = comm.get_ec_by_interval(partition_a_id, set_a);
  ec_a = ec_a.intersection(comm.get_ec_by_partition_id(partition_b_id));

  logging::sbg_log << "Node " << a_idx << ", " << set_a << " ec: " << ec_a << " and ic: " << ic_a << endl;

  auto ic_b = comm.get_ic_by_interval(partition_b_id, set_b);
  auto ec_b = comm.get_ec_by_interval(partition_b_id, set_b);
  ec_b = ec_b.intersection(comm.get_ec_by_partition_id(partition_a_id));

  logging::sbg_log << "Node " << b_idx << ", " << set_b << " ec: " << ec_b << " and ic: " << ic_b << endl;

  auto ec_edges = ec_a.cup(ec_b).difference(ec_a.intersection(ec_b));  // disjointCup does not seem to be working
  auto ic_edges = ic_a.cup(ic_b);

  int gain = ec_edges.cardinal() - ic_edges.cardinal();
  return GainObjectImbalance(a_idx, b_idx, gain, ec_a, ic_a, set_a.cardinal(), ec_b, ic_b, set_b.cardinal());
}

pair<GainObjectImbalance, CostMatrixImbalance> generate_gain_matrix(const WeightedSBGraph& graph, ICommunicationCost& cost_matrix,
                                                                    unsigned partition_a_id, Partition& partition_a,
                                                                    unsigned partition_b_id, Partition& partition_b, unsigned LMin,
                                                                    unsigned LMax)
{
  SBG::Util::Internal::TimeProfiler profiler("generate_gain_matrix");
  // create the max_gain object with a dummy initialization, any gain will be greater than -infinity
  optional<GainObjectImbalance> max_gain = nullopt;
  CostMatrixImbalance local_cost_matrix;

  for (size_t i = 0; i < partition_a.size(); i++) {
    for (size_t j = 0; j < partition_b.size(); j++) {
      auto gain_obj = generate_gain_object(i, partition_a_id, partition_a, j, partition_b_id, partition_b);
      local_cost_matrix.push_back(move(gain_obj));

      if ((not max_gain) or local_cost_matrix.back().gain > max_gain->gain) {
        max_gain = local_cost_matrix.back();
      }
    }
  }

  assert(max_gain);

  return {*max_gain, local_cost_matrix};
}

// Partition a and b (A_c and B_c in the definition) are the remining nodes to be visited, not the actual partitions
pair<pair<Set, Set>, pair<Set, Set>> update_sets(Partition& partition_a, Partition& partition_b, Set& current_moved_partition_a,
                                                 Set& current_moved_partition_b, const GainObjectImbalance& gain_object,
                                                 const WeightedSBGraph& graph)
{
  auto node_a = SET_FACT.createSet(partition_a[gain_object.a_idx]);
  size_t partition_size_a = get_node_size(node_a, graph.get_node_weights());
  bool node_a_is_fully_used = partition_size_a == gain_object.size_a;
  Set rest_a = SET_FACT.createSet();
  if (not node_a_is_fully_used) {
    tie(node_a, rest_a) = cut_interval_by_dimension(node_a, graph.get_node_weights(), gain_object.size_a);
    logging::sbg_log << "cut_interval_by_dimension " << gain_object.size_a << ": " << node_a << rest_a << endl;
  }

  auto node_b = SET_FACT.createSet(partition_b[gain_object.b_idx]);
  size_t partition_size_b = get_node_size(node_b, graph.get_node_weights());
  bool node_b_is_fully_used = partition_size_b == gain_object.size_b;
  Set rest_b = SET_FACT.createSet();
  if (not node_b_is_fully_used) {
    tie(node_b, rest_b) = cut_interval_by_dimension(node_b, graph.get_node_weights(), gain_object.size_b);
    logging::sbg_log << "cut_interval_by_dimension " << gain_object.size_b << ": " << node_b << rest_b << endl;
  }

  auto update_partition = [](Partition& partition, const Set& set) {
    for (Partition::iterator it = partition.begin(); it != partition.end(); ++it) {
      auto set_p = SET_FACT.createSet(*it);
      if (not set_p.intersection(set).isEmpty()) {
        set_p = set_p.difference(set);
        if (set_p.isEmpty()) {
          partition.erase(it);
        } else {
          *it = *set_p.begin();
        }
        break;
      }
    }
  };
  logging::sbg_log << "we remove " << node_a << " from " << partition_a << " and we get: ";
  update_partition(partition_a, node_a);
  logging::sbg_log << partition_a << endl;
  logging::sbg_log << "we remove " << node_b << " from " << partition_b << " and we get: ";
  update_partition(partition_b, node_b);
  logging::sbg_log << partition_b << endl;

  current_moved_partition_a = current_moved_partition_a.cup(node_a);
  current_moved_partition_b = current_moved_partition_b.cup(node_b);

  return make_pair(make_pair(node_a, rest_a), make_pair(node_b, rest_b));
}

Set get_set_comm(const Set& s, const WeightedSBGraph& graph)
{
  auto d1 = graph.map1().preImage(s);
  auto d2 = graph.map2().preImage(s);
  auto d = d1.cup(d2).difference(d1.intersection(d2));
  return d;
}

Set get_part_internal_comm(const Set& s, const WeightedSBGraph& graph)
{
  auto d1 = graph.map1().preImage(s);
  auto im1 = graph.map2().image(d1);
  auto internal_im1 = im1.intersection(s);
  auto e1 = graph.map2().preImage(internal_im1);
  e1 = e1.intersection(d1);

  auto d2 = graph.map2().preImage(s);
  auto im2 = graph.map1().image(d2);
  auto internal_im2 = im2.intersection(s);
  auto e2 = graph.map1().preImage(internal_im2);
  e2 = e2.intersection(d2);

  return e1.cup(e2);
}

Set get_part_comm(const Set& s, const WeightedSBGraph& graph)
{
  auto d1 = graph.map1().preImage(s);
  auto d2 = graph.map2().preImage(s);
  auto d = d1.cup(d2);
  return d;
}

GainObjectImbalance update_diff(CostMatrixImbalance& cost_matrix, const Partition& remaining_partition_a, const Set& moved_from_partition_a,
                                pair<Set, Set> affected_node_a, const Partition& remaining_partition_b, const Set& moved_from_partition_b,
                                pair<Set, Set> affected_node_b, const WeightedSBGraph& graph, const NodeWeight& node_weight,
                                const GainObjectImbalance& gain_object, const Set& ic_part_a, const Set& ic_part_b, const Set& ec,
                                unsigned LMin, unsigned LMax)
{
  SBG::Util::Internal::TimeProfiler profiler("update_diff");
  logging::sbg_log << affected_node_a.first << ", " << affected_node_a.second << endl;
  logging::sbg_log << affected_node_b.first << ", " << affected_node_b.second << endl;

  if (cost_matrix.empty()) {
    logging::sbg_log << "Cost matrix is empty, nothing to update." << endl;
    return gain_object;
  }

  // Firstly, check if indexes need fixing. Three possible causes.
  bool node_a_fully_used = affected_node_a.second.cardinal() == 0;
  bool node_b_fully_used = affected_node_b.second.cardinal() == 0;

  // all the interval was used
  // fix indexes:
  if (node_a_fully_used or node_b_fully_used) {
    CostMatrixImbalance new_cost_matrix;
    for (auto g : cost_matrix) {
      if (node_a_fully_used and g.a_idx == gain_object.a_idx) {
        continue;
      }

      if (node_b_fully_used and g.b_idx == gain_object.b_idx) {
        continue;
      }

      if (node_a_fully_used and g.a_idx > gain_object.a_idx) {
        g.a_idx--;
      }

      if (node_b_fully_used and g.b_idx > gain_object.b_idx) {
        g.b_idx--;
      }

      new_cost_matrix.push_back(move(g));
    }
    cost_matrix = new_cost_matrix;
  }

  // now, update ic and ec according to the last changes
  CostMatrixImbalance new_cost_matrix;

  if (cost_matrix.empty()) {
    logging::sbg_log << "After updating cost matrix is empty, nothing to update." << endl;
    return gain_object;
  }

  // using a reference to copy the element only once when returning
  optional<GainObjectImbalance> max_gain_object = nullopt;
  for (auto g : cost_matrix) {
    bool change = false;

    if (((not node_a_fully_used) and g.a_idx == gain_object.a_idx) or ((not node_b_fully_used) and g.b_idx == gain_object.b_idx)) {
      // node a was not fully used so we need to update ec and ic

      auto set_piece_a = remaining_partition_a.at(g.a_idx);
      auto set_piece_b = remaining_partition_b.at(g.b_idx);
      if (set_piece_a.cardinal() != set_piece_b.cardinal()) {
        unsigned size = min(set_piece_b.cardinal(), set_piece_a.cardinal());
        set_piece_a = cut_interval(set_piece_a, set_piece_a.begin()->begin() + size - 1).first;
        set_piece_b = cut_interval(set_piece_b, set_piece_b.begin()->begin() + size - 1).first;
      }

      auto set_a = SET_FACT.createSet(set_piece_a);
      auto set_b = SET_FACT.createSet(set_piece_b);

      auto ic_a = get_set_comm(set_a, graph).intersection(ic_part_a);
      auto ec_a = get_set_comm(set_a, graph).intersection(ec);

      // cout << "Node " << g.a_idx << ", " << set_a << " ec: " << ec_a << " and ic: " << ic_a << endl;

      auto ic_b = get_set_comm(set_b, graph).intersection(ic_part_b);
      auto ec_b = get_set_comm(set_b, graph).intersection(ec);

      auto ec_edges = ec_a.cup(ec_b).difference(ec_a.intersection(ec_b));  // disjointCup does not seem to be working
      auto ic_edges = ic_a.cup(ic_b);

      int gain = ec_edges.cardinal() - ic_edges.cardinal();
      g = GainObjectImbalance(g.a_idx, g.b_idx, gain, ec_a, ic_a, set_a.cardinal(), ec_b, ic_b, set_b.cardinal());
      // cout << "new gain " << g << endl;
      new_cost_matrix.push_back(move(g));
      if ((not max_gain_object) or new_cost_matrix.back().gain > max_gain_object->gain) {
        max_gain_object = new_cost_matrix.back();
      }
      continue;
    }

    // recompute gain
    auto& comm = get_communication_cost();
    auto comm_aff_a = comm.get_set_piece_edges((*affected_node_a.first.begin())[0]);
    auto comm_aff_b = comm.get_set_piece_edges((*affected_node_b.first.begin())[0]);
    // cout << "comm a: " << affected_node_a.first << " " << comm_aff_a << endl;
    // cout << "comm b: " << affected_node_b.first << " " << comm_aff_b << endl;

    auto a = remaining_partition_a.at(g.a_idx);
    auto set_g_a = cut_interval(a, a[0].begin() + g.size_a - 1).first;
    auto comm_a = comm.get_set_piece_edges(set_g_a);
    // cout << "actual comm of" << set_g_a << ": " << comm_a << endl;
    g.ec_nodes_a = g.ec_nodes_a.difference(comm_aff_b).cup(comm_a.intersection(comm_aff_a));
    g.ic_nodes_a = g.ic_nodes_a.difference(comm_aff_a).cup(comm_a.intersection(comm_aff_b));

    auto b = remaining_partition_b.at(g.b_idx);
    auto set_g_b = cut_interval(b, b[0].begin() + g.size_b - 1).first;
    auto comm_b = comm.get_set_piece_edges(set_g_b);
    // cout << "actual comm of "<< set_g_b << ": " << comm_b << endl;
    g.ec_nodes_b = g.ec_nodes_b.difference(comm_aff_a).cup(comm_b.intersection(comm_aff_b));
    g.ic_nodes_b = g.ic_nodes_b.difference(comm_aff_b).cup(comm_b.intersection(comm_aff_a));

    auto ec_nodes = g.ec_nodes_a.cup(g.ec_nodes_b).difference(g.ec_nodes_a.intersection(g.ec_nodes_b));
    g.gain = ec_nodes.cardinal() - g.ic_nodes_a.cup(g.ic_nodes_b).cardinal();

    new_cost_matrix.push_back(move(g));

    if ((not max_gain_object) or new_cost_matrix.back().gain > max_gain_object->gain) {
      max_gain_object = new_cost_matrix.back();
    }
  }
  cost_matrix = new_cost_matrix;

  assert(max_gain_object);

#if PARTITION_IMBALANCE_DEBUG
  logging::sbg_log << remaining_partition_a << ", " << remaining_partition_b << ", " << gain_object << ", " << cost_matrix << endl;
#endif

  return *max_gain_object;
}

void update_sum(int& par_sum, int g, int& max_par_sum, pair<Set, Set>& max_par_sum_set, const Set& a_v, const Set& b_v)
{
  par_sum += g;
  if (par_sum > max_par_sum) {
    max_par_sum = par_sum;
    max_par_sum_set = make_pair(a_v, b_v);
  }
}

Set get_me_edgecut(const Partition& partition_a, const Partition& partition_b, const WeightedSBGraph& graph)
{
  auto d = graph.map1().preImage(from_vector(partition_a));
  auto im = graph.map2().image(d);
  auto ec_nodes = im.intersection(from_vector(partition_b));
  auto external_communication = graph.map2().preImage(ec_nodes);
  // avoid oversizing external communication
  external_communication = external_communication.intersection(d);

  auto d2 = graph.map2().preImage(from_vector(partition_a));
  auto im2 = graph.map1().image(d2);
  auto ec_nodes2 = im2.intersection(from_vector(partition_b));
  auto external_communication2 = graph.map1().preImage(ec_nodes2);
  // avoid oversizing external communication
  external_communication2 = external_communication2.intersection(d2);

  external_communication = external_communication.cup(external_communication2);

  return external_communication;
}

int kl_sbg_imbalance(const WeightedSBGraph& graph, ICommunicationCost& cost_matrix, unsigned partition_a_id, Partition& partition_a,
                     unsigned partition_b_id, Partition& partition_b, unsigned LMin, unsigned LMax)
{
#if PARTITION_IMBALANCE_DEBUG
  logging::sbg_log << "Algorithm starts with " << partition_a << ", " << partition_b << endl;
#endif
  auto a_c = partition_a;
  auto b_c = partition_b;
  int max_par_sum = 0;
  auto max_par_sum_set = make_pair(SET_FACT.createSet(), SET_FACT.createSet());
  int par_sum = 0;
  Set a_v = SET_FACT.createSet();
  Set b_v = SET_FACT.createSet();
  const auto node_weights = graph.get_node_weights();

  auto [g, gm] = generate_gain_matrix(graph, cost_matrix, partition_a_id, partition_a, partition_b_id, partition_b, LMin, LMax);

  auto& comm = get_communication_cost();
  auto ic_a = get_part_internal_comm(from_vector(partition_a), graph);
  auto ic_b = get_part_internal_comm(from_vector(partition_b), graph);
  auto ec = comm.get_ec_by_partition_id(partition_a_id).intersection(comm.get_ec_by_partition_id(partition_b_id));

#if PARTITION_IMBALANCE_DEBUG
  logging::sbg_log << LMin << ", " << LMax << gm << endl;
#endif

#if TEST_EDGE_CUT
  int prev_edge_cut = get_me_edgecut(partition_a, partition_b, graph).cardinal();
  logging::sbg_log << "previous edge cut: " << prev_edge_cut << "\nparts: " << partition_a << " , " << partition_b << endl;
#endif

  while ((not a_c.empty()) and (not b_c.empty())) {
    logging::sbg_log << "inside the while " << a_c << ", " << b_c << " ";
    logging::sbg_log << get_partition_size(a_c, node_weights) << ", " << get_partition_size(b_c, node_weights) << endl;
    logging::sbg_log << gm << endl;

    assert(not gm.empty());

    logging::sbg_log << g << endl;

    pair<Set, Set> a_ = {SET_FACT.createSet(), SET_FACT.createSet()}, b_ = {SET_FACT.createSet(), SET_FACT.createSet()};
    tie(a_, b_) = update_sets(a_c, b_c, a_v, b_v, g, graph);

    // update part comm
    auto edges_moved_from_a = get_set_comm(a_.first, graph);
    auto ic_moved_from_a = ic_a.intersection(edges_moved_from_a);
    auto ec_moved_from_a = ec.intersection(edges_moved_from_a);

    auto edges_moved_from_b = get_set_comm(b_.first, graph);
    auto ic_moved_from_b = ic_b.intersection(edges_moved_from_b);
    auto ec_moved_from_b = ec.intersection(edges_moved_from_b);

    auto ec_moved = ec_moved_from_a.cup(ec_moved_from_b).difference(ec_moved_from_a.intersection(ec_moved_from_b));
    auto ic_moved = ic_moved_from_a.cup(ic_moved_from_b);

    ec = ec.difference(ec_moved).cup(ic_moved);
    ic_a = ic_a.difference(ic_moved_from_a);
    ic_b = ic_b.cup(ec_moved_from_a);

    ic_b = ic_b.difference(ic_moved_from_b);
    ic_a = ic_a.cup(ec_moved_from_b);

    auto new_max_gain = update_diff(gm, a_c, a_v, a_, b_c, b_v, b_, graph, node_weights, g, ic_a, ic_b, ec, LMin, LMax);
    update_sum(par_sum, g.gain, max_par_sum, max_par_sum_set, a_v, b_v);
#if TEST_EDGE_CUT
    // create a local scope to avoid mixing things
    {
      auto partition_a_set = from_vector(a_c).cup(b_v);
      flatten_set(partition_a_set, graph);
      Partition temp_partition_a;
      for_each(partition_a_set.begin(), partition_a_set.end(), [&temp_partition_a](auto s) { temp_partition_a.push_back(s); });

      auto partition_b_set = from_vector(b_c).cup(a_v);
      flatten_set(partition_b_set, graph);
      Partition temp_partition_b;
      for_each(partition_b_set.begin(), partition_b_set.end(), [&temp_partition_b](auto s) { temp_partition_b.push_back(s); });

      int current_edge_cut = get_me_edgecut(temp_partition_a, temp_partition_b, graph).cardinal();
      int expected_gain = prev_edge_cut - current_edge_cut;
      if (expected_gain != par_sum) {
        cout << "we expect " << expected_gain /*<< " from " << prev_edge_cut << " and " << current_edge_cut*/ << " but we get " << par_sum
             << " we are " << ((expected_gain == par_sum) ? "OK" : "bad") << endl;
        // cout << "parts: " << temp_partition_a << ", " <<  temp_partition_b << endl;
      }
    }
#endif
    g = move(new_max_gain);
  }

  if (max_par_sum > 0) {
    auto partition_a_set = from_vector(partition_a);
    partition_a_set = partition_a_set.difference(max_par_sum_set.first);
    partition_a_set = partition_a_set.cup(max_par_sum_set.second);
    flatten_set(partition_a_set, graph);
    partition_a.clear();
    for_each(partition_a_set.begin(), partition_a_set.end(), [&partition_a](auto s) { partition_a.push_back(s); });

    auto partition_b_set = from_vector(partition_b);
    partition_b_set = partition_b_set.difference(max_par_sum_set.second);
    partition_b_set = partition_b_set.cup(max_par_sum_set.first);
    flatten_set(partition_b_set, graph);
    partition_b.clear();
    for_each(partition_b_set.begin(), partition_b_set.end(), [&partition_b](auto s) { partition_b.push_back(s); });
  }

#if PARTITION_IMBALANCE_DEBUG
  logging::sbg_log << "so it ends with " << max_par_sum << ", " << partition_a << ", " << partition_b << endl;
#endif
  return max_par_sum;
}

KLBipartResult kl_sbg_bipart_imbalance(const WeightedSBGraph& graph, ICommunicationCost& cost_matrix, unsigned partition_a_id,
                                       Partition& partition_a, unsigned partition_b_id, Partition& partition_b, unsigned LMin,
                                       unsigned LMax)
{
  int gain = kl_sbg_imbalance(graph, cost_matrix, partition_a_id, partition_a, partition_b_id, partition_b, LMin, LMax);

#if PARTITION_IMBALANCE_DEBUG
  logging::sbg_log << "Final: " << partition_a << ", " << partition_b << endl;
#endif

  return KLBipartResult{partition_a, partition_b, gain};
}

KLSbgPartitionerResult kl_sbg_partitioner_function(const WeightedSBGraph& graph, PartitionMap& partitions, ICommunicationCost& cost_matrix,
                                                   unsigned LMin, unsigned LMax, list<KLSbgPartitionerResult>& gains)
{
  KLSbgPartitionerResult best_gain = KLSbgPartitionerResult{0, 0, -1, {}, {}};
  for (size_t i = 0; i < partitions.size(); i++) {
    const auto ec_partition_i = cost_matrix.get_ec_by_partition_id(i);
    for (size_t j = i + 1; j < partitions.size(); j++) {
      if (ec_partition_i.intersection(cost_matrix.get_ec_by_partition_id(j)).isEmpty()) {
        logging::sbg_log << "No connections between " << partitions.at(i) << " and " << partitions.at(j) << endl;
        continue;
      }

      auto gain_comp = [i, j](const KLSbgPartitionerResult& g) { return (g.i == i and g.j == j) or (g.i == j and g.j == i); };

      auto gain_it = find_if(gains.begin(), gains.end(), gain_comp);
      if (gain_it != gains.end()) {
        logging::sbg_log << "Between " << i << " and " << j << " was already computed, " << *gain_it << endl;
        continue;
      }

      cout << "Refining bisection for partitions " << i << ", " << j << endl;

      auto p_1_copy = partitions.at(i);
      auto p_2_copy = partitions.at(j);
      KLBipartResult current_gain = kl_sbg_bipart_imbalance(graph, cost_matrix, i, p_1_copy, j, p_2_copy, LMin, LMax);
#if PARTITION_IMBALANCE_DEBUG
      logging::sbg_log << "current_gain " << current_gain << endl;
#endif
      gains.emplace_back(KLSbgPartitionerResult{i, j, current_gain.gain, current_gain.A, current_gain.B});
    }
  }

  for_each(gains.begin(), gains.end(), [&best_gain](const KLSbgPartitionerResult& current_gain) {
    if (current_gain.gain > best_gain.gain) {
      best_gain = current_gain;
    }
  });

  return best_gain;
}

KLSbgPartitionerResult kl_sbg_partitioner_multithreading(const WeightedSBGraph& graph, PartitionMap& partitions,
                                                         ICommunicationCost& cost_matrix, unsigned LMin, unsigned LMax,
                                                         list<KLSbgPartitionerResult>& gains)
{
  list<future<KLSbgPartitionerResult>> workers;
  KLSbgPartitionerResult best_gain = KLSbgPartitionerResult{0, 0, -1, {}, {}};
  for (size_t i = 0; i < partitions.size(); i++) {
    const auto ec_partition_i = cost_matrix.get_ec_by_partition_id(i);
    for (size_t j = i + 1; j < partitions.size(); j++) {
      if (ec_partition_i.intersection(cost_matrix.get_ec_by_partition_id(j)).isEmpty()) {
        logging::sbg_log << "No connections between " << partitions.at(i) << " and " << partitions.at(j) << endl;
        continue;
      }

      auto gain_comp = [i, j](const KLSbgPartitionerResult& g) { return (g.i == i and g.j == j) or (g.i == j and g.j == i); };

      auto gain_it = find_if(gains.begin(), gains.end(), gain_comp);
      if (gain_it != gains.end()) {
        logging::sbg_log << "Between " << i << " and " << j << " was already computed, " << *gain_it << endl;
        continue;
      }

      auto p_1_copy = partitions.at(i);
      auto p_2_copy = partitions.at(j);
      auto th = async([&graph, &cost_matrix, &partitions, i, j, LMin, LMax]() {
        auto p_1_copy = partitions.at(i);
        auto p_2_copy = partitions.at(j);
        KLBipartResult results = kl_sbg_bipart_imbalance(graph, cost_matrix, i, p_1_copy, j, p_2_copy, LMin, LMax);
        return KLSbgPartitionerResult{i, j, results.gain, results.A, results.B};
      });
      workers.push_back(move(th));
    }
  }

  for_each(workers.begin(), workers.end(), [&best_gain, &gains](future<KLSbgPartitionerResult>& th) {
    // here we wait for each thread to finish and get its results
    auto current_gain = th.get();
    gains.emplace_back(current_gain);
  });

  for_each(gains.begin(), gains.end(), [&best_gain](const KLSbgPartitionerResult& current_gain) {
    if (current_gain.gain > best_gain.gain) {
      best_gain = current_gain;
    }
  });

  return best_gain;
}

}  // namespace

ostream& operator<<(ostream& os, const KLBipartResult& result)
{
  os << "{ gain: " << result.gain << ", A: " << result.A << ", B: " << result.B << "}";

  return os;
}

ostream& operator<<(ostream& os, const GainObjectImbalance& gain)
{
  os << "< Node: " << gain.a_idx << ", size: " << gain.size_a << " - Node: " << gain.b_idx << ", size: " << gain.size_b
     << ", gain: " << gain.gain << " >";

  return os;
}

ostream& operator<<(ostream& os, const CostMatrixImbalance& cost_matrix)
{
  os << "{\n";
  for (const auto& o : cost_matrix) {
    os << "\t" << o << "\n";
  }
  os << "}";

  return os;
}

ostream& operator<<(ostream& os, const KLSbgPartitionerResult& result)
{
  os << "{ partition results: " << result.i << ", " << result.j << ", " << result.gain << ", A: " << result.A << ", B: " << result.B << "}";

  return os;
}

string get_pretty_sb_graph(const SBG::LIB::SBG& g)
{
  rapidjson::Document json_doc;
  rapidjson::Document::AllocatorType& allocator = json_doc.GetAllocator();
  json_doc.SetObject();

  rapidjson::Value obj_nodes(rapidjson::kArrayType);
  for (const SetPiece& set_piece : g.V()) {
    rapidjson::Value obj_set_piece_array(rapidjson::kArrayType);
    for (const Interval& interval : set_piece.intervals()) {
      rapidjson::Value obj_interval_array(rapidjson::kArrayType);

      rapidjson::Value begin(rapidjson::kNumberType);
      begin.SetUint(interval.begin());
      obj_interval_array.PushBack(begin, allocator);

      rapidjson::Value end(rapidjson::kNumberType);
      end.SetUint(interval.end());
      obj_interval_array.PushBack(end, allocator);

      obj_set_piece_array.PushBack(obj_interval_array, allocator);
    }

    obj_nodes.PushBack(obj_set_piece_array, allocator);
  }

  json_doc.AddMember("nodes", obj_nodes, allocator);

  auto map_parser = [&json_doc, &allocator](const auto& maps, const char* key) {
    rapidjson::Value obj_maps(rapidjson::kArrayType);
    for (const auto& map1 : maps) {
      rapidjson::Value map_1_obj(rapidjson::kObjectType);

      const Set& domain = map1.dom();
      rapidjson::Value domain_obj(rapidjson::kArrayType);
      for (const auto& interval : domain) {
        rapidjson::Value obj_interval_array(rapidjson::kArrayType);

        rapidjson::Value begin(rapidjson::kNumberType);
        begin.SetUint(interval.intervals()[0].begin());
        obj_interval_array.PushBack(begin, allocator);

        rapidjson::Value end(rapidjson::kNumberType);
        end.SetUint(interval.intervals()[0].end());
        obj_interval_array.PushBack(end, allocator);

        domain_obj.PushBack(obj_interval_array, allocator);
      }
      map_1_obj.AddMember("domain", domain_obj, allocator);

      rapidjson::Value exp_obj(rapidjson::kArrayType);
      for (const auto& exp : map1.exp()) {
        rapidjson::Value obj_exp_array(rapidjson::kArrayType);

        rapidjson::Value slope(rapidjson::kNumberType);
        float _slope = exp.slope().numerator();
        slope.SetFloat(_slope);
        obj_exp_array.PushBack(slope, allocator);

        rapidjson::Value offset(rapidjson::kNumberType);
        float _offset = exp.offset().numerator();
        slope.SetFloat(_offset);
        obj_exp_array.PushBack(offset, allocator);

        exp_obj.PushBack(obj_exp_array, allocator);
      }
      map_1_obj.AddMember("exp", exp_obj, allocator);

      obj_maps.PushBack(map_1_obj, allocator);
    }

    rapidjson::Value key_value(key, allocator);

    json_doc.AddMember(key_value, obj_maps, allocator);
  };

  map_parser(g.map1(), "map1");
  map_parser(g.map2(), "map2");

  // Write the JSON data to the file
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);
  json_doc.Accept(writer);

  string json_data = string(s.GetString());

  return json_data;
}

void kl_sbg_imbalance_partitioner(const WeightedSBGraph& graph, PartitionMap& partitions, const float imbalance_epsilon,
                                  const bool enable_multithreading)
{
  auto [LMin, LMax] =
      imbalance_epsilon > 0.0 ? compute_lmin_lmax(graph, partitions.size(), imbalance_epsilon) : make_pair<unsigned, unsigned>(0, 0);
  bool change = true;
  int counter = 0;

  ICommunicationCost& cost_matrix = get_communication_cost();
  list<KLSbgPartitionerResult> gains;
  while (change) {
    cout << "*****ITERATION NUMBER " << counter++ << endl;
    change = false;

    KLSbgPartitionerResult best_gain;
    if (enable_multithreading) {
      best_gain = kl_sbg_partitioner_multithreading(graph, partitions, cost_matrix, LMin, LMax, gains);
    } else {
      best_gain = kl_sbg_partitioner_function(graph, partitions, cost_matrix, LMin, LMax, gains);
    }

    logging::sbg_log << "Best gain results is: " << best_gain << endl;

    auto gain_comp = [&best_gain](const KLSbgPartitionerResult& g) {
      return g.i == best_gain.i or g.j == best_gain.j or g.i == best_gain.j or g.j == best_gain.i;
    };

    // now, apply changes
    constexpr int strategy = 2;
    // first strategy
    switch (strategy) {
    case 1:
      if (best_gain.gain > 0) {
        change = true;
        partitions[best_gain.i] = best_gain.A;
        partitions[best_gain.j] = best_gain.B;

        gains.erase(std::remove_if(gains.begin(), gains.end(), gain_comp), gains.end());
      }

      break;
    default:

      int it_counter = 0;
      list<size_t> modified_partitions = {};
      while (not gains.empty() and best_gain.gain > 0) {
        logging::sbg_log << "change number " << it_counter << " changing " << best_gain.i << ", " << best_gain.j << endl;
        it_counter++;
        change = true;
        partitions[best_gain.i] = best_gain.A;
        partitions[best_gain.j] = best_gain.B;
        modified_partitions.emplace_back(best_gain.i);
        modified_partitions.emplace_back(best_gain.j);

        gains.erase(std::remove_if(gains.begin(), gains.end(), gain_comp), gains.end());

        logging::sbg_log << "best gain is " << best_gain << endl;
        logging::sbg_log << "and vector is ";
#ifdef SBG_PARTITIONER_LOGGING
        for_each(gains.begin(), gains.end(), [](const KLSbgPartitionerResult& g) { logging::sbg_log << g << " "; });
#endif
        logging::sbg_log << endl;

        if (not gains.empty()) {
          auto max_gain_it = max_element(gains.begin(), gains.end(),
                                         [](const KLSbgPartitionerResult& a, const KLSbgPartitionerResult& b) { return a.gain < b.gain; });
          if (max_gain_it != gains.end()) {
            best_gain = *max_gain_it;
          } else {
            best_gain = KLSbgPartitionerResult{0, 0, -Inf, {}, {}};
          }
        }
      }

      cost_matrix.update_partitions(partitions, modified_partitions);
      break;
    }
  }
}

}  // namespace sbg_partitioner
