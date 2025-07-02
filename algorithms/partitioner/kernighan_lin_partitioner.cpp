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
#include <bitset>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <unordered_map>

#include "build_sb_graph.hpp"
#include "cost.hpp"
#include "kernighan_lin_partitioner.hpp"
#include "sbg_partitioner_log.hpp"


#define PARTITION_IMBALANCE_DEBUG 1
#define PARTITION_IMBALANCE_PROFILE 0


// This code is based on https://github.com/CIFASIS/sbg-partitioner/discussions/17


using namespace std;

using namespace SBG::LIB;
using namespace SBG::Util;

namespace sbg_partitioner {


using ec_ic = std::pair<Set , Set>;

// Using unnamed namespace to define functions with internal linkage
namespace {

constexpr bool multithreading_enabled = false;

pair<unsigned, unsigned>
compute_lmin_lmax(const WeightedSBGraph& graph, unsigned number_of_partitions, const float imbalance_epsilon, SetAF& set_fact)
{
    unsigned w_v = get_node_size(graph.V(), graph.get_node_weights(), set_fact);
    unsigned B = ceil(w_v / number_of_partitions);
    int im = imbalance_epsilon * B;
    unsigned LMin = B - im;
    unsigned LMax = B + im;

    return make_pair(LMin, LMax);
}


size_t get_c_ab(
    const Set& a, const Set& b,
    const PWMap& map_1,
    const PWMap& map_2,
    const EdgeCost& costs,
    SetAF& set_fact)
{
    auto f = [&set_fact](auto& a, auto& b, const PWMap& map_1, const PWMap& map_2) {
        auto d = map_1.preImage(a);
        auto im = map_2.image(d);
        auto inters = b.intersection(im);
        auto comm_edges = map_2.preImage(inters);
        comm_edges = comm_edges.intersection(d);

        return comm_edges;
    };

    auto intersection1 = f(a, b, map_1, map_2);
    auto intersection2 = f(a, b, map_2, map_1);

    auto communication_edges = intersection1.cup(intersection2);

    size_t comm_size = get_edge_set_cost(communication_edges, costs);

    return comm_size;
}



ec_ic compute_EC_IC(
    const Partition& partition,
    const Set& nodes,
    const Partition& partition_2,
    const PWMap& map_1,
    const PWMap& map_2,
    SetAF& set_fact)
{
    auto d = map_1.preImage(nodes);
    auto im = map_2.image(d);
    auto partition_set = from_vector(partition, set_fact);
    auto ic_nodes = partition_set.intersection(im);
    ic_nodes = ic_nodes.difference(nodes);
    auto ec_nodes = im.difference(ic_nodes);
    auto partition_2_set = from_vector(partition_2, set_fact);
    ec_nodes = ec_nodes.intersection(partition_2_set);
    auto ic = map_2.preImage(ic_nodes).intersection(d);
    auto ec = map_2.preImage(ec_nodes).intersection(d);

    return make_pair(ec, ic);
}


unsigned get_imbalance_size(unsigned min_imbal_part, unsigned max_imbal_part, unsigned size_node_a, unsigned size_node_b, unsigned current_moved_size)
{
    logging::sbg_log << "nodes_imbal_part = " << max_imbal_part << ", " << size_node_a << endl;
    unsigned nodes_imbal_part = size_node_a > 0 ? floor(max_imbal_part / size_node_a) : 0;

    unsigned min_nodes_imbal_part = size_node_b > 0 ? floor(min_imbal_part / size_node_b) : 0;

    if (nodes_imbal_part > min_imbal_part) {
        nodes_imbal_part = min_nodes_imbal_part;
    }

    unsigned remaining_node_a = unsigned(size_node_a - current_moved_size);
    logging::sbg_log << "min between " << remaining_node_a << " and " << current_moved_size << endl;
    nodes_imbal_part = std::min(unsigned(current_moved_size), remaining_node_a);
    unsigned new_size_a = current_moved_size + nodes_imbal_part;

    return new_size_a;
}


GainObjectImbalance get_gain(
    unsigned idx_a,
    Set& nodes_a,
    const Partition& partition_a,
    unsigned size_a,
    unsigned idx_b,
    Set& nodes_b,
    const Partition& partition_b,
    unsigned size_b,
    const WeightedSBGraph& graph,
    const NodeWeight& node_weight,
    SetAF& set_fact)
{
    Set a = set_fact.createSet(), b = set_fact.createSet(), rest_a = set_fact.createSet(), rest_b = set_fact.createSet();
    tie(a, rest_a) = cut_interval_by_dimension(nodes_a, node_weight, size_a, set_fact);
    tie(b, rest_b) = cut_interval_by_dimension(nodes_b, node_weight, size_b, set_fact);

    // Now, compute external and internal cost for both maps
    Set ec_nodes_a_1 = set_fact.createSet(), ic_nodes_a_1 = set_fact.createSet();
    tie(ec_nodes_a_1, ic_nodes_a_1) = compute_EC_IC(partition_a, a, partition_b, graph.map1(), graph.map2(), set_fact);

    Set ec_nodes_a_2 = set_fact.createSet(), ic_nodes_a_2 = set_fact.createSet();
    tie(ec_nodes_a_2, ic_nodes_a_2) = compute_EC_IC(partition_a, a, partition_b, graph.map2(), graph.map1(), set_fact);

    // Get the union between both external and internal costs for both combination of maps
    Set ec_nodes_a = set_fact.createSet(), ic_nodes_a = set_fact.createSet();
    ec_nodes_a = ec_nodes_a_1.cup(ec_nodes_a_2);
    ic_nodes_a = ic_nodes_a_1.cup(ic_nodes_a_2);

    logging::sbg_log << "Node " << idx_a << ", " << nodes_a << " ec: " << ec_nodes_a << " and ic: " << ic_nodes_a << endl;

    size_t ec_a = get_edge_set_cost(ec_nodes_a, graph.get_edge_costs());
    size_t ic_a = get_edge_set_cost(ic_nodes_a, graph.get_edge_costs());
    int d_a = ec_a - ic_a;

    // Same as before for partition b
    Set ec_nodes_b_1 = set_fact.createSet(), ic_nodes_b_1 = set_fact.createSet();
    tie(ec_nodes_b_1, ic_nodes_b_1) = compute_EC_IC(partition_b, b, partition_a, graph.map1(), graph.map2(), set_fact);

    Set ec_nodes_b_2 = set_fact.createSet(), ic_nodes_b_2 = set_fact.createSet();
    tie(ec_nodes_b_2, ic_nodes_b_2) = compute_EC_IC(partition_b, b, partition_a, graph.map2(), graph.map1(), set_fact);

    Set ec_nodes_b = set_fact.createSet(), ic_nodes_b = set_fact.createSet();
    ec_nodes_b = ec_nodes_b_1.cup(ec_nodes_b_2);
    ic_nodes_b = ic_nodes_b_1.cup(ic_nodes_b_2);

    logging::sbg_log << "Node " << idx_b << ", " << nodes_b << " ec: " << ec_nodes_b << " and ic: " << ic_nodes_b << endl;

    size_t ec_b = get_edge_set_cost(ec_nodes_b, graph.get_edge_costs());
    size_t ic_b = get_edge_set_cost(ic_nodes_b, graph.get_edge_costs());
    int d_b = ec_b - ic_b;

    // Get communication between a and b
    size_t c_ab = get_c_ab(a, b, graph.map1(), graph.map2(), graph.get_edge_costs(), set_fact);

    // calculate gain
    int gain = d_a + d_b - 2 * c_ab;

    auto gain_obj = GainObjectImbalance{idx_a, idx_b, gain, ec_nodes_a, ic_nodes_a, size_a, ec_nodes_b, ic_nodes_b, size_b};

    return gain_obj;
}


void compute_exchange(unsigned i, unsigned j, Partition& partition_a, unsigned current_size_a,
    Partition& partition_b, unsigned current_size_b, const WeightedSBGraph& graph, const NodeWeight& node_weight,
    unsigned LMin, unsigned LMax, CostMatrixImbalance& cost_matrix, SetAF& set_fact)
{
    auto nodes_a = set_fact.createSet(partition_a[i]);
    auto nodes_b = set_fact.createSet(partition_b[j]);

    // Take the minimum partition size. We add 1 because it includes the last element
    size_t size_node_a = get_node_size(nodes_a, node_weight, set_fact);
    size_t size_node_b = get_node_size(nodes_b, node_weight, set_fact);
    size_t min_size = min(size_node_a, size_node_b);

    // No problem here, a is just a copy of partition_a[i], same for b
    GainObjectImbalance gain_obj = get_gain(i, nodes_a, partition_a, min_size, j, nodes_b, partition_b, min_size, graph, node_weight, set_fact);

    // gain is greater than 0 and we are not moving all elements of node_a
    bool is_imbalance_enabled = LMin > 0 or LMax > 0;
    bool is_gain_positive = gain_obj.gain > 0;
    if (is_imbalance_enabled and is_gain_positive and size_node_a > min_size) {
        unsigned max_imbal_part = LMax > current_size_b ? LMax - current_size_b : 0; // this is what we can move to b

        unsigned min_imbal_part = current_size_a > LMin ? current_size_a - LMin : 0; // this is what we can move from a

        unsigned new_size_a = get_imbalance_size(min_imbal_part, max_imbal_part, size_node_a, size_node_b, min_size);

        GainObjectImbalance gain_obj_imbalance = get_gain(i, nodes_a, partition_a, new_size_a, j, nodes_b, partition_b, min_size, graph, node_weight, set_fact);

        logging::sbg_log << "is gain better? " << gain_obj << ", " << gain_obj_imbalance << endl;

        if (gain_obj_imbalance.gain > gain_obj.gain) {
            gain_obj = move(gain_obj_imbalance);
        }
    }

    // gain is greater than 0 and we are not moving all elements of node_b
    if (is_imbalance_enabled and is_gain_positive and size_node_b > min_size) {
        unsigned max_imbal_part = LMax > current_size_a ? LMax - current_size_a : 0; // this is what we can move to a

        unsigned min_imbal_part = current_size_b > LMin ? current_size_b - LMin : 0; // this is what we can move from b

        unsigned new_size_b = get_imbalance_size(min_imbal_part, max_imbal_part, size_node_b, size_node_a, min_size);

        GainObjectImbalance gain_obj_imbalance = get_gain(i, nodes_a, partition_a, min_size, j, nodes_b, partition_b, new_size_b, graph, node_weight, set_fact);

        logging::sbg_log << "is gain better? " << gain_obj << ", " << gain_obj_imbalance << endl;

        if (gain_obj_imbalance.gain > gain_obj.gain) {
            gain_obj = move(gain_obj_imbalance);
        }
    }

    cost_matrix.emplace(move(gain_obj));
}


CostMatrixImbalance generate_gain_matrix(
    const WeightedSBGraph& graph,
    CostMatrix& cost_matrix,
    unsigned partition_a_id,
    Partition& partition_a,
    unsigned partition_b_id,
    Partition& partition_b,
    unsigned LMin,
    unsigned LMax,
    SetAF& set_fact)
{
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

            auto ec_edges = ec_i_a.cup(ec_j_b).difference(ec_i_a.intersection(ec_j_b)); // disjointCup does not seem to be working
            auto ic_edges = ic_i_a.cup(ic_j_b);

            int gain = ec_edges.cardinal() - ic_edges.cardinal();
            auto gain_object = GainObjectImbalance{ i, j, gain, ec_i_a, ic_i_a, set_i_a.cardinal(), ec_j_b, ic_j_b, set_j_b.cardinal() };
            local_cost_matrix.insert(gain_object);
        }
    }

    return local_cost_matrix;
}


// Partition a and b (A_c and B_c in the definition) are the remining nodes to be visited, not the actual partitions
pair<pair<Set, Set>, pair<Set, Set>> update_sets(
    Partition& partition_a,
    Partition& partition_b,
    Set& current_moved_partition_a,
    Set& current_moved_partition_b,
    const GainObjectImbalance& gain_object,
    const WeightedSBGraph& graph,
    SetAF& set_fact)
{
    auto node_a = set_fact.createSet(partition_a[gain_object.a_idx]);
    size_t partition_size_a = get_node_size(node_a, graph.get_node_weights(), set_fact);
    bool node_a_is_fully_used = partition_size_a == gain_object.size_a;
    Set rest_a = set_fact.createSet();
    if (not node_a_is_fully_used) {
        tie(node_a, rest_a) = cut_interval_by_dimension(node_a, graph.get_node_weights(), gain_object.size_a, set_fact);
        logging::sbg_log << "cut_interval_by_dimension " << gain_object.size_a << ": " << node_a << rest_a << endl;
    }

    auto node_b = set_fact.createSet(partition_b[gain_object.b_idx]);
    size_t partition_size_b = get_node_size(node_b, graph.get_node_weights(), set_fact);
    bool node_b_is_fully_used = partition_size_b == gain_object.size_b;
    Set rest_b = set_fact.createSet();
    if (not node_b_is_fully_used) {
        tie(node_b, rest_b) = cut_interval_by_dimension(node_b, graph.get_node_weights(), gain_object.size_b, set_fact);
        logging::sbg_log << "cut_interval_by_dimension " << gain_object.size_b << ": " << node_b << rest_b << endl;
    }

    auto update_partition = [&set_fact](Partition& partition, const Set& set) {
        for (Partition::iterator it = partition.begin(); it != partition.end(); ++it) {
            auto set_p = set_fact.createSet(*it);
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


void update_diff(
    CostMatrixImbalance& cost_matrix,
    Partition& remaining_partition_a,
    Set& moved_from_partition_a,
    pair<Set, Set> affected_node_a,
    Partition& remaining_partition_b,
    Set& moved_from_partition_b,
    pair<Set, Set> affected_node_b,
    const WeightedSBGraph& graph,
    const NodeWeight& node_weight,
    const GainObjectImbalance& gain_object,
    unsigned LMin,
    unsigned LMax,
    SetAF& set_fact)
{
    logging::sbg_log << affected_node_a.first << ", " << affected_node_a.second << endl;
    logging::sbg_log << affected_node_b.first << ", " << affected_node_b.second << endl;

    // Firstly, check if indexes need fixing. Three possible causes.
    size_t affected_node_a_size = get_node_size(affected_node_a.second, node_weight, set_fact);
    bool node_a_fully_used = affected_node_a_size == 0;

    size_t affected_node_b_size = get_node_size(affected_node_b.second, node_weight, set_fact);
    bool node_b_fully_used = affected_node_b_size == 0;

    unsigned size_a = get_partition_size(remaining_partition_a, node_weight, set_fact);
    size_a += get_node_size(moved_from_partition_b, node_weight, set_fact);

    unsigned size_b = get_partition_size(remaining_partition_b, node_weight, set_fact);
    size_b += get_node_size(moved_from_partition_a, node_weight, set_fact);

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

            new_cost_matrix.insert(g);
        }
        cost_matrix = new_cost_matrix;
    }

    // now, update ic and ec according to the last changes
    auto affected_nodes = affected_node_a.first.cup(affected_node_b.first);
    auto discarded_edges = graph.map1().preImage(affected_nodes).cup(graph.map2().preImage(affected_nodes));
    CostMatrixImbalance new_cost_matrix;
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

        new_cost_matrix.insert(g);
    }
    cost_matrix = new_cost_matrix;

#if PARTITION_IMBALANCE_DEBUG
    logging::sbg_log << remaining_partition_a << ", " << remaining_partition_b << ", " << gain_object << ", " << cost_matrix << endl;
#endif
}


// auto return type we’ll let the compiler deduce what the return type should be from the return statement
auto max_diff(CostMatrixImbalance& cost_matrix)
{
    // cost_matrix is sort by gain, so the first is the maximum gain
    auto g = cost_matrix.begin();

    auto gain_object = *g;
#if PARTITION_IMBALANCE_DEBUG
    logging::sbg_log << "The best is " << *g << endl;
#endif

    // remove it, we need to update those values that
    cost_matrix.erase(g);

    return gain_object;
}


void update_sum(
    int& par_sum,
    int g,
    int& max_par_sum,
    pair<Set, Set>& max_par_sum_set,
    const Set& a_v,
    const Set& b_v)
{
    par_sum += g;
    if (par_sum > max_par_sum) {
        max_par_sum = par_sum;
        max_par_sum_set = make_pair(a_v, b_v);
    }
}


int kl_sbg_imbalance(
    const WeightedSBGraph& graph,
    CostMatrix& cost_matrix,
    unsigned partition_a_id,
    Partition& partition_a,
    unsigned partition_b_id,
    Partition& partition_b,
    unsigned LMin,
    unsigned LMax,
    SetAF& set_fact)
{
#if PARTITION_IMBALANCE_DEBUG
    logging::sbg_log << "Algorithm starts with " << partition_a << ", " << partition_b << endl;
#endif
    auto a_c = partition_a;
    auto b_c = partition_b;
    int max_par_sum = 0;
    auto max_par_sum_set = make_pair(set_fact.createSet(), set_fact.createSet());
    int par_sum = 0;
    Set a_v = set_fact.createSet();
    Set b_v = set_fact.createSet();
    const auto node_weights = graph.get_node_weights();

    auto start_generate_gain_matrix = chrono::high_resolution_clock::now();
    CostMatrixImbalance gm = generate_gain_matrix(graph, cost_matrix, partition_a_id, partition_a, partition_b_id, partition_b, LMin, LMax, set_fact);
    auto end_generate_gain_matrix = chrono::high_resolution_clock::now();
    auto time_to_generate_gain_matrix = chrono::duration<double, std::milli>(end_generate_gain_matrix - start_generate_gain_matrix).count();
    cout << "time_to_generate_gain_matrix: " << time_to_generate_gain_matrix << endl;


#if PARTITION_IMBALANCE_DEBUG
        logging::sbg_log << LMin << ", "
             << LMax
             << gm << endl;
#endif

    while ((not a_c.empty()) and (not b_c.empty())) {
        logging::sbg_log << "inside the while " << a_c << ", " << b_c << " ";
        logging::sbg_log << get_partition_size(a_c, node_weights, set_fact) << ", " << get_partition_size(b_c, node_weights, set_fact) << endl;
        logging::sbg_log << gm << endl;
        assert(not gm.empty());
        GainObjectImbalance g = max_diff(gm);
        logging::sbg_log << g << endl;
        pair<Set, Set> a_ = {set_fact.createSet(), set_fact.createSet()}, b_ = {set_fact.createSet(), set_fact.createSet()};
        tie(a_, b_) = update_sets(a_c, b_c, a_v, b_v, g, graph, set_fact);
        auto start_update_diff = chrono::high_resolution_clock::now();
        update_diff(gm, a_c, a_v, a_, b_c, b_v, b_, graph, node_weights, g, LMin, LMax, set_fact);
        auto end_update_diff = chrono::high_resolution_clock::now();
        auto time_to_update_diff = chrono::duration<double, std::milli>(end_update_diff - start_update_diff).count();
        cout << "time_to_update_diff: " << time_to_update_diff << endl;
        update_sum(par_sum, g.gain, max_par_sum, max_par_sum_set, a_v, b_v);
    }

    if (max_par_sum > 0) {
        
        auto partition_a_set = from_vector(partition_a, set_fact);
        partition_a_set = partition_a_set.difference(max_par_sum_set.first);
        partition_a_set = partition_a_set.cup(max_par_sum_set.second);
        flatten_set(partition_a_set, graph, set_fact);
        partition_a.clear();
        for_each(partition_a_set.begin(), partition_a_set.end(), [&partition_a](auto s) { partition_a.push_back(s); });

        auto partition_b_set = from_vector(partition_b, set_fact);
        partition_b_set = partition_b_set.difference(max_par_sum_set.second);
        partition_b_set = partition_b_set.cup(max_par_sum_set.first);
        flatten_set(partition_b_set, graph, set_fact);
        partition_b.clear();
        for_each(partition_b_set.begin(), partition_b_set.end(), [&partition_b](auto s) { partition_b.push_back(s); });
    }

#if PARTITION_IMBALANCE_DEBUG
    logging::sbg_log << "so it ends with " << max_par_sum << ", " << partition_a << ", " << partition_b << endl;
#endif
    return max_par_sum;
}


KLBipartResult kl_sbg_bipart_imbalance(const WeightedSBGraph& graph, CostMatrix& cost_matrix, unsigned partition_a_id,
    Partition& partition_a, unsigned partition_b_id, Partition& partition_b, unsigned LMin, unsigned LMax, SetAF& set_fact)
{
    int gain = kl_sbg_imbalance(graph, cost_matrix, partition_a_id, partition_a, partition_b_id, partition_b, LMin, LMax, set_fact);

#if PARTITION_IMBALANCE_DEBUG
    logging::sbg_log << "Final: " << partition_a << ", " << partition_b << endl;
#endif

    return KLBipartResult{partition_a, partition_b, gain};
}


kl_sbg_partitioner_result kl_sbg_partitioner_function(
    const WeightedSBGraph& graph, PartitionMap& partitions, CostMatrix& cost_matrix, unsigned LMin, unsigned LMax,
    vector<kl_sbg_partitioner_result>& gains, SetAF& set_fact, MapAF& map_fact)
{
    kl_sbg_partitioner_result best_gain = kl_sbg_partitioner_result{ 0, 0, -1, {}, {}};
    for (size_t i = 0; i < partitions.size(); i++) {
        const auto ec_partition_i = cost_matrix.get_ec_by_partition_id(i);
        for (size_t j = i + 1; j < partitions.size(); j++) {

            if (ec_partition_i.intersection(cost_matrix.get_ec_by_partition_id(j)).isEmpty()) {
                logging::sbg_log << "No connections between " << partitions.at(i) << " and " << partitions.at(j) << endl;
                continue;
            }

            auto gain_comp = [i, j] (const kl_sbg_partitioner_result& g) {
                return (g.i == i and g.j == j) or (g.i == j and g.j == i);
            };

            auto gain_it = find_if(gains.begin(), gains.end(), gain_comp);
            if (gain_it != gains.end()) {
                logging::sbg_log << "Between " << i << " and " << j << " was already computed, " << *gain_it  << endl;
                continue;
            }

            auto p_1_copy = partitions.at(i);
            auto p_2_copy = partitions.at(j);
            KLBipartResult current_gain = kl_sbg_bipart_imbalance(graph, cost_matrix, i, p_1_copy, j, p_2_copy, LMin, LMax, set_fact);
    #if PARTITION_IMBALANCE_DEBUG
            logging::sbg_log << "current_gain " << current_gain << endl;
    #endif
            gains.emplace_back(kl_sbg_partitioner_result{ i, j, current_gain.gain, current_gain.A, current_gain.B });
        }
    }

    for_each(gains.begin(), gains.end(), [&best_gain](const kl_sbg_partitioner_result& current_gain) {
        if (current_gain.gain > best_gain.gain) {
            best_gain = current_gain;
        }
    });

    return best_gain;
}


kl_sbg_partitioner_result kl_sbg_partitioner_multithreading(
    const WeightedSBGraph& graph, PartitionMap& partitions, unsigned LMin, unsigned LMax,
    vector<kl_sbg_partitioner_result>& gains, SetAF& set_fact)
{
        // avoid repeating this
    // auto look_for_adjacents = [&set_fact](const Set& nodes, const PWMap& map1, const PWMap& map2) {
    //     auto involved_edges1 = map1.preImage(nodes);
    //     auto arrival_nodes1 = map2.image(involved_edges1);
    //     arrival_nodes1 = arrival_nodes1.difference(nodes);

    //     auto involved_edges2 = map2.preImage(nodes);
    //     auto arrival_nodes2 = map1.image(involved_edges2);
    //     arrival_nodes2 = arrival_nodes2.difference(nodes);

    //     auto arrival_nodes = arrival_nodes1.cup(arrival_nodes2);

    //     return arrival_nodes;
    // };


    kl_sbg_partitioner_result best_gain = kl_sbg_partitioner_result{ 0, 0, -1, {}, {}};
    // vector<future<kl_sbg_partitioner_result>> workers;
    // map<size_t, Set> adjacents;
    // for (size_t i = 0; i < partitions.size(); i++) {

    //     if (adjacents.find(i) == adjacents.end()) {
    //         adjacents.insert_or_assign(i, look_for_adjacents(from_vector(partitions.at(i), set_fact), graph.map1(), graph.map2()));
    //     }

    //     for (size_t j = i + 1; j < partitions.size(); j++) {

    //         if (adjacents.at(i).intersection(from_vector(partitions.at(j), set_fact)).isEmpty()) {
    //             logging::sbg_log << "No connections between " << partitions[i] << " and " << partitions[j] << " is empty" << endl;
    //             continue;
    //         }

    //         auto gain_comp = [i, j] (const kl_sbg_partitioner_result& g) {
    //             return (g.i == i and g.j == j) or (g.i == j and g.j == i);
    //         };

    //         auto gain_it = find_if(gains.begin(), gains.end(), gain_comp);
    //         if (gain_it != gains.end()) {
    //             logging::sbg_log << "Between " << i << " and " << j << " was already computed, " << *gain_it  << endl;
    //             continue;
    //         }

    //         auto th = async([&graph, &partitions, i, j, LMin, LMax, &set_fact] () {
    //             auto p_1_copy = partitions.at(i);
    //             auto p_2_copy = partitions.at(j);
    //             KLBipartResult results = kl_sbg_bipart_imbalance(graph, p_1_copy, p_2_copy, LMin, LMax, set_fact);
    //             return kl_sbg_partitioner_result{i, j, results.gain, results.A, results.B};
    //         });
    //         workers.push_back(move(th));
    //     }
    // }

    // for_each(workers.begin(), workers.end(), [&best_gain, &gains] (future<kl_sbg_partitioner_result>& th) {
    //     // here we wait for each thread to finish and get its results
    //     auto current_gain = th.get();
    //     gains.emplace_back(current_gain);
    // });

    // for_each(gains.begin(), gains.end(), [&best_gain] (const kl_sbg_partitioner_result& current_gain) {
    //     if (current_gain.gain > best_gain.gain) {
    //         best_gain = current_gain;
    //     }
    // });

    return best_gain;
}


}


ostream& operator<<(ostream& os, const KLBipartResult& result)
{
    os << "{ gain: " << result.gain << ", A: " << result.A << ", B: " << result.B << "}";

    return os;
}


ostream& operator<<(ostream& os, const GainObjectImbalance& gain)
{
    os << "< Node: "
       << gain.a_idx
       << ", size: "
       << gain.size_a
       << " - Node: "
       << gain.b_idx
       << ", size: "
       << gain.size_b
       << ", gain: "
       << gain.gain
       << " >";

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


ostream& operator<<(ostream& os, const kl_sbg_partitioner_result& result)
{
    os << "{ partition results: "
       << result.i
       << ", "
       << result.j
       << ", "
       << result.gain
       << ", A: "
       << result.A
       << ", B: "
       << result.B
       << "}";

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

    auto map_parser = [&json_doc, &allocator] (const auto& maps, const char* key) {
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


void kl_sbg_imbalance_partitioner(
    const WeightedSBGraph& graph, PartitionMap& partitions, const float imbalance_epsilon, SetAF& set_fact, MapAF& map_fact)
{
    auto [LMin, LMax] = imbalance_epsilon > 0.0 ? compute_lmin_lmax(graph, partitions.size(), imbalance_epsilon, set_fact) : make_pair<unsigned, unsigned>(0, 0);
    bool change = true;
    int counter = 0;

    CostMatrix cost_matrix = CostMatrix(graph, partitions, set_fact);
    vector<kl_sbg_partitioner_result> gains;
    while (change) {
        cout << "*****ITERATION NUMBER " << counter++ << endl;
        change = false;

        kl_sbg_partitioner_result best_gain;
        if (multithreading_enabled) {
            best_gain = kl_sbg_partitioner_multithreading(graph, partitions, LMin, LMax, gains, set_fact);
        } else {
            best_gain = kl_sbg_partitioner_function(graph, partitions, cost_matrix, LMin, LMax, gains, set_fact, map_fact);
        }

        logging::sbg_log << "Best gain results is: " << best_gain << endl;

        auto gain_comp = [&best_gain](const kl_sbg_partitioner_result& g) {
            return g.i == best_gain.i or g.j == best_gain.j
                or g.i == best_gain.j or g.j == best_gain.i;
        };

        // now, apply changes
        constexpr int strategy = 2;
        // first strategy
        switch (strategy)
        {
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
            while (not gains.empty() and best_gain.gain > 0) {
                logging::sbg_log << "change number " << it_counter << " changing " << best_gain.i << ", " << best_gain.j << endl;
                it_counter++;
                change = true;
                partitions[best_gain.i] = best_gain.A;
                partitions[best_gain.j] = best_gain.B;

                gains.erase(std::remove_if(gains.begin(), gains.end(), gain_comp), gains.end());

                logging::sbg_log << "best gain is " << best_gain << endl;
                logging::sbg_log << "and vector is ";
                #ifdef SBG_PARTITIONER_LOGGING
                for_each(gains.begin(), gains.end(), [](const kl_sbg_partitioner_result& g) { logging::sbg_log << g << " "; });
                #endif
                logging::sbg_log << endl;

                if (not gains.empty()){
                    auto max_gain_it = max_element(gains.begin(), gains.end(),
                        [] (const kl_sbg_partitioner_result& a, const kl_sbg_partitioner_result& b) {
                            return a.gain < b.gain;
                        });
                    if (max_gain_it != gains.end()) {
                        best_gain = *max_gain_it;
                    } else {
                        best_gain = kl_sbg_partitioner_result{ 0, 0, -Inf, {}, {}} ;
                    }
                }
            }

            cost_matrix.update_partitions(partitions);
            break;
        }
    }
}

}