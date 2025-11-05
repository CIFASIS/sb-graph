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

#include <bits/stdc++.h>
#include <set>
#include <utility>
#include <util/logger.hpp>

#include "build_sb_graph.hpp"
#include "communication_cost.hpp"
#include "dfs_on_sbg.hpp"
#include "partition_graph.hpp"
#include "partition_graph_cc.hpp"
#include "sbg_partitioner_log.hpp"
#include "sbg_partitioner_types.hpp"

using namespace std;
using namespace SBG::LIB;

namespace sbg_partitioner {

namespace using_cc {


ostream& operator<<(ostream& os, const SetPointer& set_pointer)
{
    os << "[" << set_pointer.index << ", " << set_pointer.set_piece << ", " << set_pointer.size << "]";

    return os;
}


ostream& operator<<(ostream& os, const SetPointers& set_pointers)
{
    os << "{";
    for (const auto& s : set_pointers) {
        os << s << " ";
    }
    os << "}";

    return os;
}


struct GainObject {
    SetPointer a;
    SetPointer b;
    size_t size;
    float gain;
    float alpha_a;
    float alpha_b;

    GainObject(SetPointer a_idx, SetPointer b_idx, size_t size, float gain, float alpha_a, float alpha_b)
    : a(move(a_idx)),
      b(move(b_idx)),
      size(size),
      gain(gain),
      alpha_a(alpha_a),
      alpha_b(alpha_b)
    {}

    bool operator==(const GainObject& other) {
        return a == other.a and
               b == other.b and
               size == other.size and
               gain == other.gain;
    }
};

typedef vector<GainObject> GainObjects;


ostream& operator<<(ostream& os, const GainObject& gain_obj)
{
    os << "{ "
       << gain_obj.a
       << ", "
       << gain_obj.b
       << ", "
       << gain_obj.size
       << ", "
       << gain_obj.gain
       << ", "
       << gain_obj.alpha_a
       << ", "
       << gain_obj.alpha_b
       << "}";

    return os;
}


ostream& operator<<(ostream& os, const GainObjects& gains)
{
    for (const auto& g : gains) {
        os << g << "\n";
    }

    return os;
}


namespace {

float compute_D(
    const SetPointer& set_pointer,
    const SetPointers& partition,
    const SetPointers& complementary_partition,
    const CommunicationCostCC& cost_matrix_cc)
{
    float ic = 0;
    for (size_t i = 0; i < partition.size(); i++) {
        ic += cost_matrix_cc.get_communication(set_pointer.index, partition.at(i).index);
    }

    float ec = 0;
    for (size_t i = 0; i < complementary_partition.size(); i++) {
        ec += cost_matrix_cc.get_communication(set_pointer.index, complementary_partition.at(i).index);
    }

    float D = ec - ic;
    return D;
}


unordered_map<unsigned, float> compute_Ds(const SetPointers& partition,
    const SetPointers& complementary_partition,
    const CommunicationCostCC& cost_matrix_cc)
{
    unordered_map<unsigned, float> Ds = {};

    for (size_t i = 0; i < partition.size(); i++) {
        float D = compute_D(partition.at(i), partition, complementary_partition, cost_matrix_cc);
        Ds.insert({partition.at(i).index, D});
    }

    return Ds;
}

}


tuple<optional<GainObject>, GainObjects> compute_gains(
    const SetPointers& partition_a,
    const SetPointers& partition_b,
    CommunicationCostCC& cost_matrix_cc,
    const unordered_map<unsigned, float>& Ds_a,
    const unordered_map<unsigned, float>& Ds_b)
{
    GainObjects gains;
    int max_gain_index = -1;

    for (size_t i = 0; i < partition_a.size(); i++) {

        auto p_a_i = partition_a.at(i);
        float D_i = Ds_a.at(p_a_i.index);
    
        for (size_t j = 0; j < partition_b.size(); j++) {

            auto p_b_j = partition_b.at(j);
            float D_j = Ds_b.at(p_b_j.index);

            int s = min(p_a_i.size, p_b_j.size);

            float alpha_i = s / float(p_a_i.set_piece.cardinal());
            float alpha_j = s / float(p_b_j.set_piece.cardinal());

            float gain = D_i * alpha_i + D_j * alpha_j;
            gain -= 2 * cost_matrix_cc.get_communication(p_a_i.index, p_b_j.index) * min(alpha_i, alpha_j);

            gains.emplace_back(p_a_i, p_b_j, s, gain, alpha_i, alpha_j);

            if (max_gain_index < 0 or gains.at(max_gain_index).gain < gain) {
                max_gain_index = gains.size() - 1;
            }
        }
    }

    if (max_gain_index < 0) {
        return { nullopt, gains };
    }

    return { gains.at(max_gain_index), gains };
}


namespace {

void remove_elements_from_partition(SetPointers& partition, const SetPointers& max_sum_set)
{
    for (const auto& p : max_sum_set) {
        logging::sbg_log << "moving from a " << p << endl;
        auto it = find_if(partition.begin(), partition.end(), [&p](const auto& s) { return s.index == p.index; } );
        assert(it != partition.end());
        logging::sbg_log << "found " << *it << endl;
        if (it->size == p.size) {
            partition.erase(it);
        } else {
            logging::sbg_log << "sizes " << it->size << ", " << p.size << endl;
            it->size -= p.size;
            logging::sbg_log << "new size " << it->size << endl;
            logging::sbg_log << partition << endl;
        }
    }
}


void add_elements_to_partition(SetPointers& partition, const SetPointers& max_sum_set)
{
    for (const auto& p : max_sum_set) {
        assert(p.size > 0);
        auto it = find_if(partition.begin(), partition.end(), [&p](const auto& s) { return s.index == p.index; } );
        if (it == partition.end()) {
            partition.push_back(p);
        } else {
            logging::sbg_log << "add_elements_to_partition " << *it << ", " << p.size << " ";
            it->size += p.size;
            logging::sbg_log << partition << endl;
        }
    }
}

}


int bisection(
    const SBG::LIB::WeightedSBGraph& graph,
    CommunicationCostCC& cost_matrix,
    SetPointers& partition_a,
    SetPointers& partition_b,
    unsigned LMin,
    unsigned LMax)
{
    logging::sbg_log << "calling bisection with\n" << partition_a << "\n" << partition_b << endl;
    auto partition_a_copy = partition_a;
    auto partition_b_copy = partition_b;

    int par_sum = 0;
    int max_par_sum = 0;
    pair<SetPointers, SetPointers> max_par_sum_set = {};
    unordered_map<unsigned, float> Ds_a = compute_Ds(partition_a, partition_b, cost_matrix);
    unordered_map<unsigned, float> Ds_b = compute_Ds(partition_b, partition_a, cost_matrix);
    auto [max_gain, gm] = compute_gains(partition_a_copy, partition_b_copy, cost_matrix, Ds_a, Ds_b);

    if (not max_gain) {
        return 0;
    }

    SetPointers a_v, b_v;

    assert(gm.size() > 0);

    while (not partition_a_copy.empty() and not partition_b_copy.empty()) {
        logging::sbg_log << gm << endl;
        logging::sbg_log << "max_gain " << *max_gain << endl;

        // save partial sets and sum
        par_sum += max_gain->gain;
        a_v.emplace_back(max_gain->a.index, max_gain->a.set_piece, max_gain->size);
        b_v.emplace_back(max_gain->b.index, max_gain->b.set_piece, max_gain->size);
        if (par_sum > max_par_sum) {
            max_par_sum = par_sum;
            max_par_sum_set = make_pair(a_v, b_v);
        }

        // remove chosen elements for partition a
        partition_a_copy.erase(remove(partition_a_copy.begin(), partition_a_copy.end(), max_gain->a));
        if (max_gain->size < max_gain->a.size) {
            partition_a_copy.emplace_back(max_gain->a.index, max_gain->a.set_piece, max_gain->a.size - max_gain->size);
        }

        // remove chosen elements for partition b
        partition_b_copy.erase(remove(partition_b_copy.begin(), partition_b_copy.end(), max_gain->b));
        if (max_gain->size < max_gain->b.size) {
            partition_b_copy.emplace_back(max_gain->b.index, max_gain->b.set_piece, max_gain->b.size - max_gain->size);
        }

        // update gains
        if (not partition_a_copy.empty() and not partition_b_copy.empty()) {
            Ds_a = compute_Ds(partition_a_copy, partition_b_copy, cost_matrix);
            Ds_b = compute_Ds(partition_b_copy, partition_a_copy, cost_matrix);

            tie(max_gain, gm) = compute_gains(partition_a_copy, partition_b_copy, cost_matrix, Ds_a, Ds_b);
        }
    }

#ifdef SBG_PARTITIONER_LOGGING
    auto prev_a = partition_a;
    auto prev_b = partition_b;
#endif
    remove_elements_from_partition(partition_a, max_par_sum_set.first);
    remove_elements_from_partition(partition_b, max_par_sum_set.second);

    add_elements_to_partition(partition_a, max_par_sum_set.second);
    add_elements_to_partition(partition_b, max_par_sum_set.first);

#ifdef SBG_PARTITIONER_LOGGING
    logging::sbg_log << "result:\nmax par sum: " << max_par_sum  << "\nprev_a: " << prev_a << "\nprev_b: " << prev_b << "\nmax_par_set: " << max_par_sum_set.first << "\n" << max_par_sum_set.second << "\npartition a: " << partition_a << "\npartition b: " << partition_b << endl;
#endif

    return max_par_sum;
}


PartitionMap rebuild_partitions(const SetPointers& sorted_nodes, const vector<SetPointers>& partitions)
{
    vector<unsigned> pivots(sorted_nodes.size(), 0);
    for (const auto& s : sorted_nodes) {
        pivots[s.index] = s.set_piece.begin()[0].begin();
    }

    PartitionMap new_partitions;
    new_partitions.reserve(partitions.size());
    for (const auto& partition : partitions) {
        new_partitions.emplace_back();
        for (const auto& set_pointer : partition) {
            unsigned begin = pivots[set_pointer.index];
            new_partitions.back().push_back(Interval(begin, 1, begin + set_pointer.size - 1));
            pivots[set_pointer.index] += set_pointer.size;
        }
    }

    return new_partitions;
}


BisectionResult refine_partitions_function(const WeightedSBGraph& graph, const vector<SetPointers>& partitions,
                                           CommunicationCostCC& cost_matrix, unsigned LMin, unsigned LMax, list<BisectionResult>& gains)
{
    BisectionResult best_gain = BisectionResult{0, 0, -1, {}, {}};
    for (size_t i = 0; i < partitions.size(); i++) {
        for (size_t j = i + 1; j < partitions.size(); j++) {
            auto gain_comp = [i, j](const BisectionResult& g) { return (g.i == i and g.j == j) or (g.i == j and g.j == i); };

            auto gain_it = find_if(gains.begin(), gains.end(), gain_comp);
            if (gain_it != gains.end()) {
                logging::sbg_log << "Between " << i << " and " << j << " was already computed, " << gain_it->gain << endl;
                continue;
            }

            auto p_1_copy = partitions.at(i);
            auto p_2_copy = partitions.at(j);
            int current_gain = bisection(graph, cost_matrix, p_1_copy, p_2_copy, LMin, LMax);
            logging::sbg_log << "current_gain for " << i << ", " << j << " is " << current_gain << endl;
            gains.emplace_back(BisectionResult{i, j, current_gain, p_1_copy, p_2_copy});
        }
    }

    for_each(gains.begin(), gains.end(), [&best_gain](const BisectionResult& current_gain) {
        if (current_gain.gain > best_gain.gain) {
            best_gain = current_gain;
        }
    });

    return best_gain;
}


void kl_sbg_imbalance_partitioner(const WeightedSBGraph& graph, const SetPointers& sorted_nodes, vector<SetPointers>& partitions,
                                  CommunicationCostCC& cost_matrix, const float imbalance_epsilon)
{
    auto [LMin, LMax] = make_pair<unsigned, unsigned>(0, 0);
    bool change = true;
    int counter = 0;

    list<BisectionResult> gains;
    while (change) {
        logging::sbg_log << "*****ITERATION NUMBER " << counter++ << endl;
        change = false;

        BisectionResult best_gain;
        gains.clear();
        best_gain = refine_partitions_function(graph, partitions, cost_matrix, LMin, LMax, gains);

        logging::sbg_log << "Best gain results is: " << best_gain.gain << endl;
        logging::sbg_log << "How many gains: " << gains.size() << endl;

        auto gain_comp = [&best_gain](const BisectionResult& g) {
            return g.i == best_gain.i or g.j == best_gain.j or g.i == best_gain.j or g.j == best_gain.i;
        };

        int it_counter = 0;
        while ((not gains.empty()) and best_gain.gain > 0) {
            logging::sbg_log << "change number " << it_counter << " changing " << best_gain.i << ", " << best_gain.j << endl;
            it_counter++;
            change = true;
            partitions[best_gain.i] = best_gain.A;
            partitions[best_gain.j] = best_gain.B;

            gains.erase(std::remove_if(gains.begin(), gains.end(), gain_comp), gains.end());
            gains.clear();

            logging::sbg_log << "best gain is " << best_gain.gain << endl;
            logging::sbg_log << "gains length " << gains.size() << endl;

            if (not gains.empty()) {
                auto max_gain_it = max_element(gains.begin(), gains.end(),
                                            [](const BisectionResult& a, const BisectionResult& b) { return a.gain < b.gain; });
                assert(max_gain_it != gains.end());
                if (max_gain_it != gains.end()) {
                    best_gain = *max_gain_it;
                } else {
                    best_gain = BisectionResult{0, 0, -Inf, {}, {}};
                }
            }
        }
    }
}


vector<SetPointers> best_initial_partition(
    const WeightedSBGraph& graph,
    const SetPointers& sorted_nodes,
    const CommunicationCostCC& comm_cc,
    unsigned number_of_partitions,
    const InitialPartitionStrategy strategy)
{
    unsigned edge_cut = 0;
    vector<SetPointers> partitions;
    partitions.reserve(number_of_partitions);

    const bool all_strategies = strategy == InitialPartitionStrategy::ALL;
    if (all_strategies or strategy == InitialPartitionStrategy::DFS_GREEDY_POSTORDER or strategy == InitialPartitionStrategy::DFS_GREEDY_PREORDER) {
        auto nodes_to_add = sorted_nodes;
        float expected_size_ = float(graph.V().cardinal());
        expected_size_ /= number_of_partitions;
        unsigned expected_size = ceil(expected_size_);

        for (unsigned i = 0; i < number_of_partitions; i++) {
            unsigned current_size = 0;
            partitions.emplace_back();
            for (auto it = nodes_to_add.begin(); it != nodes_to_add.end();) {
                if (it->size + current_size > expected_size) {
                    unsigned s = expected_size - current_size;
                    partitions[i].emplace_back(it->index, it->set_piece, s);
                    it->size -= s;
                    current_size += s;
                    // we don't update the iterator in this case
                } else {
                    partitions[i].push_back(*it);
                    current_size += it->size;
                    it = nodes_to_add.erase(it);
                }

                if (current_size >= expected_size and i < number_of_partitions - 1) {
                    break;
                }
            }
        }

#ifdef SBG_PARTITIONER_LOGGING
        auto partitions_map = sbg_partitioner::using_cc::rebuild_partitions(sorted_nodes, partitions);
        sanity_check(graph, partitions_map, number_of_partitions);
#endif

        for (size_t i = 0; i < number_of_partitions; i++) {
            const auto& p_i = partitions.at(i);
            for (size_t j = i + 1; j < number_of_partitions; j++) {
                const auto& p_j = partitions.at(j);
                for (const auto& set_piece_i : p_i) {
                    for (const auto& set_piece_j : p_j) {
                        edge_cut += comm_cc.get_communication(set_piece_i.index, set_piece_j.index);
                    }    
                }
            }
        }
    }

    if (all_strategies or strategy == InitialPartitionStrategy::DFS_DISTRIBUTIVE_PREORDER or strategy == InitialPartitionStrategy::DFS_DISTRIBUTIVE_POSTORDER) {
        vector<unsigned> size_by_partition(number_of_partitions, 0);
        vector<SetPointers> temp_partition = vector<SetPointers>(number_of_partitions, SetPointers());

        for (auto v : sorted_nodes) {
            int quotient = v.size / number_of_partitions;
            int rest = v.size % number_of_partitions;
            logging::sbg_log << v << " size " << v.size << " quotient " << quotient << ", rest " << rest << endl;

            if (quotient > 0) {
                for (size_t i = 0; i < number_of_partitions; i++) {
                    temp_partition[i].emplace_back(v.index, v.set_piece, quotient);
                    size_by_partition[i] += quotient;
                }
            }

            if (rest > 0) {
                unsigned min_size = 0;
                for (size_t i = 1; i < number_of_partitions; i++) {
                    if (size_by_partition.at(i) < size_by_partition.at(min_size)) {
                        min_size = i;
                    }
                }
            
                // if quotient is greater than 0, we need to extend one of the intervals
                if (quotient > 0) {
                    // the last one inserted is `v`, so we add rest to it
                    assert(v.set_piece == temp_partition[min_size].back().set_piece);
                    temp_partition[min_size].back().size += rest;
                    size_by_partition[min_size] += rest;
                } else { // quotient is 0, we need to insert it
                    temp_partition[min_size].push_back(v);
                }
            }
        }

#ifdef SBG_PARTITIONER_LOGGING
        auto partitions_map = sbg_partitioner::using_cc::rebuild_partitions(sorted_nodes, temp_partition);
        sanity_check(graph, partitions_map, number_of_partitions);
#endif

        unsigned this_edge_cut = 0;
        if (not partitions.empty()) {
            for (size_t i = 0; i < number_of_partitions; i++) {
                const auto& p_i = partitions.at(i);
                for (size_t j = i + 1; j < number_of_partitions; j++) {
                    const auto& p_j = partitions.at(j);
                    for (const auto& set_piece_i : p_i) {
                        for (const auto& set_piece_j : p_j) {
                            this_edge_cut += comm_cc.get_communication(set_piece_i.index, set_piece_j.index);
                        }    
                    }
                }
            }

            if (this_edge_cut < edge_cut) {
                partitions = temp_partition;
            }
        } else {
            partitions = temp_partition;
        } 
    }

#ifdef SBG_PARTITIONER_LOGGING
    auto partitions_as_set = sbg_partitioner::using_cc::rebuild_partitions(sorted_nodes, partitions);
    sanity_check(graph, partitions_as_set, number_of_partitions);
#endif

    return partitions;
}

}

}
