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

// This code is based on https://github.com/CIFASIS/sbg-partitioner/discussions/17

using namespace std;

using namespace SBG::LIB;
using namespace SBG::Util;

namespace sbg_partitioner {

using ec_ic = std::pair<Set, Set>;

// Using unnamed namespace to define functions with internal linkage
namespace {

pair<unsigned, unsigned> compute_lmin_lmax(const WeightedSBGraph& graph, unsigned number_of_partitions, const float imbalance_epsilon,
                                           const SetAF& set_fact)
{
  unsigned w_v = get_node_size(graph.V(), graph.get_node_weights(), set_fact);
  unsigned B = ceil(w_v / number_of_partitions);
  int im = imbalance_epsilon * B;
  unsigned LMin = B - im;
  unsigned LMax = B + im;

  return make_pair(LMin, LMax);
}

pair<GainObjectImbalance, CostMatrixImbalance> generate_gain_matrix(const WeightedSBGraph& graph, ICommunicationCost& cost_matrix, unsigned partition_a_id,
                                         Partition& partition_a, unsigned partition_b_id, Partition& partition_b, unsigned LMin,
                                         unsigned LMax)
{
  SBG::Util::Internal::TimeProfiler profiler("generate_gain_matrix");
  const auto& fact = graph.fact();
  // create the max_gain object with a dummy initialization, any gain will be greater than -infinity
  GainObjectImbalance max_gain = GainObjectImbalance{
    numeric_limits<size_t>::infinity(),
    numeric_limits<size_t>::infinity(),
    -numeric_limits<size_t>::infinity(),
    fact.createSet(),
    fact.createSet(),
    0,
    fact.createSet(),
    fact.createSet(),
    0
  };
  CostMatrixImbalance local_cost_matrix;

  for (size_t i = 0; i < partition_a.size(); i++) {
    for (size_t j = 0; j < partition_b.size(); j++) {
      auto set_i_a = partition_a.at(i);
      auto set_j_b = partition_b.at(j);
      if (set_i_a.cardinal() != set_j_b.cardinal()) {
        unsigned size = min(set_j_b.cardinal(), set_i_a.cardinal());
        set_i_a = cut_interval(set_i_a, set_i_a.begin()->begin() + size - 1).first;
        set_j_b = cut_interval(set_j_b, set_j_b.begin()->begin() + size - 1).first;
      }

      auto ic_i_a = cost_matrix.get_ic_by_interval(partition_a_id, set_i_a);
      auto ec_i_a = cost_matrix.get_ec_by_interval(partition_a_id, set_i_a);
      ec_i_a = ec_i_a.intersection(cost_matrix.get_ec_by_partition_id(partition_b_id));

      logging::sbg_log << "Node " << i << ", " << set_i_a << " ec: " << ec_i_a << " and ic: " << ic_i_a << endl;

      auto ic_j_b = cost_matrix.get_ic_by_interval(partition_b_id, set_j_b);
      auto ec_j_b = cost_matrix.get_ec_by_interval(partition_b_id, set_j_b);
      ec_j_b = ec_j_b.intersection(cost_matrix.get_ec_by_partition_id(partition_a_id));

      logging::sbg_log << "Node " << j << ", " << set_j_b << " ec: " << ec_j_b << " and ic: " << ic_j_b << endl;

      auto ec_edges = ec_i_a.cup(ec_j_b).difference(ec_i_a.intersection(ec_j_b));  // disjointCup does not seem to be working
      auto ic_edges = ic_i_a.cup(ic_j_b);

      int gain = ec_edges.cardinal() - ic_edges.cardinal();
      local_cost_matrix.emplace_back(i, j, gain, ec_i_a, ic_i_a, set_i_a.cardinal(), ec_j_b, ic_j_b, set_j_b.cardinal());

      if (local_cost_matrix.back().gain > max_gain.gain) {
            max_gain = local_cost_matrix.back();
      }
    }
  }

  return { max_gain, local_cost_matrix };
}

// Partition a and b (A_c and B_c in the definition) are the remining nodes to be visited, not the actual partitions
pair<pair<Set, Set>, pair<Set, Set>> update_sets(Partition& partition_a, Partition& partition_b, Set& current_moved_partition_a,
                                                 Set& current_moved_partition_b, const GainObjectImbalance& gain_object,
                                                 const WeightedSBGraph& graph)
{
  const SetAF& set_factory = graph.fact();
  auto node_a = set_factory.createSet(partition_a[gain_object.a_idx]);
  size_t partition_size_a = get_node_size(node_a, graph.get_node_weights(), set_factory);
  bool node_a_is_fully_used = partition_size_a == gain_object.size_a;
  Set rest_a = set_factory.createSet();
  if (not node_a_is_fully_used) {
    tie(node_a, rest_a) = cut_interval_by_dimension(node_a, graph.get_node_weights(), gain_object.size_a, set_factory);
    logging::sbg_log << "cut_interval_by_dimension " << gain_object.size_a << ": " << node_a << rest_a << endl;
  }

  auto node_b = set_factory.createSet(partition_b[gain_object.b_idx]);
  size_t partition_size_b = get_node_size(node_b, graph.get_node_weights(), set_factory);
  bool node_b_is_fully_used = partition_size_b == gain_object.size_b;
  Set rest_b = set_factory.createSet();
  if (not node_b_is_fully_used) {
    tie(node_b, rest_b) = cut_interval_by_dimension(node_b, graph.get_node_weights(), gain_object.size_b, set_factory);
    logging::sbg_log << "cut_interval_by_dimension " << gain_object.size_b << ": " << node_b << rest_b << endl;
  }

  auto update_partition = [&set_factory](Partition& partition, const Set& set) {
    for (Partition::iterator it = partition.begin(); it != partition.end(); ++it) {
      auto set_p = set_factory.createSet(*it);
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

GainObjectImbalance update_diff(CostMatrixImbalance& cost_matrix, Partition& remaining_partition_a, Set& moved_from_partition_a,
                 pair<Set, Set> affected_node_a, Partition& remaining_partition_b, Set& moved_from_partition_b,
                 pair<Set, Set> affected_node_b, const WeightedSBGraph& graph, const NodeWeight& node_weight,
                 const GainObjectImbalance& gain_object, unsigned LMin, unsigned LMax)
{
  SBG::Util::Internal::TimeProfiler profiler("update_diff");
  logging::sbg_log << affected_node_a.first << ", " << affected_node_a.second << endl;
  logging::sbg_log << affected_node_b.first << ", " << affected_node_b.second << endl;

  if (cost_matrix.empty()) {
    logging::sbg_log << "Cost matrix is empty, nothing to update." << endl;
    return gain_object;
  }

  const SetAF& set_factory = graph.fact();

  // Firstly, check if indexes need fixing. Three possible causes.
  size_t affected_node_a_size = get_node_size(affected_node_a.second, node_weight, set_factory);
  bool node_a_fully_used = affected_node_a_size == 0;

  size_t affected_node_b_size = get_node_size(affected_node_b.second, node_weight, set_factory);
  bool node_b_fully_used = affected_node_b_size == 0;

  unsigned size_a = get_partition_size(remaining_partition_a, node_weight, set_factory);
  size_a += get_node_size(moved_from_partition_b, node_weight, set_factory);

  unsigned size_b = get_partition_size(remaining_partition_b, node_weight, set_factory);
  size_b += get_node_size(moved_from_partition_a, node_weight, set_factory);

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
  auto affected_nodes = affected_node_a.first.cup(affected_node_b.first);
  auto discarded_edges = graph.map1().preImage(affected_nodes).cup(graph.map2().preImage(affected_nodes));
  CostMatrixImbalance new_cost_matrix;

  if (cost_matrix.empty()) {
    logging::sbg_log << "After updating cost matrix is empty, nothing to update." << endl;
    return gain_object;
  }

  // using a reference to copy the element only once when returning
  GainObjectImbalance& max_gain_object = cost_matrix.front();
  for (auto g : cost_matrix) {
    bool change = false;

    if ((not node_a_fully_used) and g.a_idx == gain_object.a_idx) {
      auto new_size = min(affected_node_a_size, g.size_b);
      g.size_a = new_size;
      g.size_b = new_size;
      change = true;
    }

    if ((not node_b_fully_used) and g.b_idx == gain_object.b_idx) {
      auto new_size = min(affected_node_b_size, g.size_a);
      g.size_a = new_size;
      g.size_b = new_size;
      change = true;
    }

    if (not g.ic_nodes_a.intersection(discarded_edges).isEmpty() or not g.ec_nodes_a.intersection(discarded_edges).isEmpty()) {
      g.ic_nodes_a = g.ic_nodes_a.difference(discarded_edges);
      g.ec_nodes_a = g.ec_nodes_a.difference(discarded_edges);
      change = true;
    }

    if (not g.ic_nodes_b.intersection(discarded_edges).isEmpty() or not g.ec_nodes_b.intersection(discarded_edges).isEmpty()) {
      g.ic_nodes_b = g.ic_nodes_b.difference(discarded_edges);
      g.ec_nodes_b = g.ec_nodes_b.difference(discarded_edges);
      change = true;
    }

    if (change) {
      auto ec_nodes = g.ec_nodes_a.cup(g.ec_nodes_b).difference(g.ec_nodes_a.intersection(g.ec_nodes_b));
      auto ic_nodes = g.ic_nodes_a.cup(g.ic_nodes_b);

      // calculate gain
      int gain = ec_nodes.cardinal() - ic_nodes.cardinal();
      g.gain = gain;
    }

    new_cost_matrix.push_back(move(g));

    if (new_cost_matrix.back().gain > max_gain_object.gain) {
      max_gain_object = new_cost_matrix.back();
    }
  }
  cost_matrix = new_cost_matrix;

#if PARTITION_IMBALANCE_DEBUG
  logging::sbg_log << remaining_partition_a << ", " << remaining_partition_b << ", " << gain_object << ", " << cost_matrix << endl;
#endif

    return max_gain_object;
}


void update_sum(int& par_sum, int g, int& max_par_sum, pair<Set, Set>& max_par_sum_set, const Set& a_v, const Set& b_v)
{
  par_sum += g;
  if (par_sum > max_par_sum) {
    max_par_sum = par_sum;
    max_par_sum_set = make_pair(a_v, b_v);
  }
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
  const SetAF& set_fact = graph.fact();
  auto max_par_sum_set = make_pair(set_fact.createSet(), set_fact.createSet());
  int par_sum = 0;
  Set a_v = set_fact.createSet();
  Set b_v = set_fact.createSet();
  const auto node_weights = graph.get_node_weights();

  auto [g, gm] = generate_gain_matrix(graph, cost_matrix, partition_a_id, partition_a, partition_b_id, partition_b, LMin, LMax);

#if PARTITION_IMBALANCE_DEBUG
  logging::sbg_log << LMin << ", " << LMax << gm << endl;
#endif

  while ((not a_c.empty()) and (not b_c.empty())) {
    logging::sbg_log << "inside the while " << a_c << ", " << b_c << " ";
    logging::sbg_log << get_partition_size(a_c, node_weights, set_fact) << ", " << get_partition_size(b_c, node_weights, set_fact) << endl;
    logging::sbg_log << gm << endl;

    assert(not gm.empty());

    logging::sbg_log << g << endl;

    pair<Set, Set> a_ = {set_fact.createSet(), set_fact.createSet()}, b_ = {set_fact.createSet(), set_fact.createSet()};
    tie(a_, b_) = update_sets(a_c, b_c, a_v, b_v, g, graph);
    auto new_max_gain = update_diff(gm, a_c, a_v, a_, b_c, b_v, b_, graph, node_weights, g, LMin, LMax);
    update_sum(par_sum, g.gain, max_par_sum, max_par_sum_set, a_v, b_v);
    g = move(new_max_gain);
  }

  if (max_par_sum > 0) {
    auto partition_a_set = from_vector(partition_a, set_fact);
    partition_a_set = partition_a_set.difference(max_par_sum_set.first);
    partition_a_set = partition_a_set.cup(max_par_sum_set.second);
    flatten_set(partition_a_set, graph);
    partition_a.clear();
    for_each(partition_a_set.begin(), partition_a_set.end(), [&partition_a](auto s) { partition_a.push_back(s); });

    auto partition_b_set = from_vector(partition_b, set_fact);
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

void kl_sbg_imbalance_partitioner(const WeightedSBGraph& graph, PartitionMap& partitions, const float imbalance_epsilon, const bool enable_multithreading)
{
  auto [LMin, LMax] = imbalance_epsilon > 0.0 ? compute_lmin_lmax(graph, partitions.size(), imbalance_epsilon, graph.fact())
                                              : make_pair<unsigned, unsigned>(0, 0);
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