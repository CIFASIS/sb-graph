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
#include "sbg_partitioner_log.hpp"

using namespace std;
using namespace SBG::LIB;

namespace sbg_partitioner {

namespace using_cc {


ostream& operator<<(ostream& os, const SetPointer& set_pointer)
{
    os << "[" << set_pointer.index << ", " << set_pointer.set_piece << "]";

    return os;
}


ostream& operator<<(ostream& os, const SetPointers& set_pointers)
{
    for (const auto& s : set_pointers) {
        os << s << " ";
    }

    return os;
}


struct GainObject {
    SetPointer a;
    SetPointer b;
    size_t size;
    int gain;

    GainObject(SetPointer a_idx, SetPointer b_idx, size_t size, int gain)
    : a(move(a_idx)),
      b(move(b_idx)),
      size(size),
      gain(gain)
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



// typedef vector<pair<unsigned, unsigned>> PartitionByIdx;


// ostream& operator<<(ostream& os, const PartitionByIdx& nodes)
// {
//     os << "[";
//     for (const auto& n : nodes) {
//         os << "(" << n.first << ", " << n.second << ") ";
//     }

//     if (nodes.size() > 0) {
//         os << "\b";
//     }
//     os << "]";

//     return os;
// }



tuple<optional<GainObject>, GainObjects> compute_gains(
    const SetPointers& partition_a,
    const SetPointers& partition_b,
    CommunicationCostCC& cost_matrix_cc)
{
    GainObjects gains;
    int max_gain_index = -1;

    for (size_t i = 0; i < partition_a.size(); i++) {

        int i_ic = 0;
        for (size_t i_c = 0; i_c < partition_a.size(); i_c++) {
            i_ic += cost_matrix_cc.get_communication(partition_a.at(i).index, partition_a.at(i_c).index);
        }

        int i_ec = 0;
        for (size_t j_c = 0; j_c < partition_b.size(); j_c++) {
            i_ec += cost_matrix_cc.get_communication(partition_a.at(i).index, partition_b.at(j_c).index);
        }

        int D_i = i_ec - i_ic;
    
        for (size_t j = 0; j < partition_b.size(); j++) {
            int j_ic = 0;
            for (size_t j_c = 0; j_c < partition_b.size(); j_c++) {
                j_ic += cost_matrix_cc.get_communication(partition_b.at(j).index, partition_b.at(j_c).index);
            }

            int j_ec = 0;
            for (size_t i_c = 0; i_c < partition_a.size(); i_c++) {
                j_ec += cost_matrix_cc.get_communication(partition_b.at(j).index, partition_a.at(i_c).index);
            }

            int D_j = j_ec - j_ic;

            int s = min(partition_a.at(i).set_piece.cardinal(), partition_b.at(j).set_piece.cardinal());

            int gain = D_i + D_j;
            gain -= 2 * cost_matrix_cc.get_communication(partition_a.at(i).index, partition_b.at(j).index);

            gains.emplace_back(partition_a.at(i), partition_b.at(j), s, gain);

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


int run_bisection(
    const SBG::LIB::WeightedSBGraph& graph,
    CommunicationCostCC& cost_matrix,
    SetPointers& partition_a,
    SetPointers& partition_b,
    const PWMap& cc_map,
    unsigned LMin,
    unsigned LMax)
{
    auto partition_a_copy = partition_a;
    auto partition_b_copy = partition_b;

    int par_sum = 0;
    int max_par_sum = 0;
    pair<SetPointers, SetPointers> max_par_sum_set = {};
    auto [max_gain, gm] = compute_gains(partition_a_copy, partition_b_copy, cost_matrix);

    if (not max_gain) {
        return 0;
    }

    SetPointers a_v, b_v;

    assert(gm.size() > 0);

    while (not partition_a_copy.empty() and not partition_b_copy.empty()) {
        par_sum += max_gain->gain;
        a_v.push_back(max_gain->a);
        b_v.push_back(max_gain->b);
        if (par_sum > max_par_sum) {
            max_par_sum = par_sum;
            max_par_sum_set = make_pair(a_v, b_v);
        }
        partition_a_copy.erase(remove(partition_a_copy.begin(), partition_a_copy.end(), max_gain->a));
        partition_b_copy.erase(remove(partition_b_copy.begin(), partition_b_copy.end(), max_gain->b));

        if (not partition_a_copy.empty() and not partition_b_copy.empty()) {
            tie(max_gain, gm) = compute_gains(partition_a_copy, partition_b_copy, cost_matrix);
        }
    }

    if (max_par_sum > 0) {
        for (const auto& a : max_par_sum_set.first) {
            partition_a.erase(remove(partition_a.begin(), partition_a.end(), a));
        }

        for (const auto& b : max_par_sum_set.second) {
            partition_b.erase(remove(partition_b.begin(), partition_b.end(), b));
        }

        partition_a.insert(partition_a.end(), max_par_sum_set.second.begin(), max_par_sum_set.second.end());
        partition_b.insert(partition_b.end(), max_par_sum_set.first.begin(), max_par_sum_set.first.end());
    }

    cout << "result:\nmax par sum: " << max_par_sum << "\npartition a: " << partition_a << "\npartition b: " << partition_b << endl;

    return max_par_sum;
}


void bisection(
    const SBG::LIB::WeightedSBGraph& graph,
    CommunicationCostCC& cost_matrix,
    SetPointers& partition_a,
    SetPointers& partition_b,
    const PWMap& cc_map,
    unsigned LMin,
    unsigned LMax)
{
    auto partition_a_copy = partition_a;
    auto partition_b_copy = partition_b;

    int gain = sbg_partitioner::using_cc::run_bisection(graph, cost_matrix, partition_a_copy, partition_b_copy, cc_map, 0, 0);

    PartitionMap partitions;
    partitions.emplace_back();
    for_each(partition_a_copy.begin(), partition_a_copy.end(), [&partitions](auto p) { partitions.back().push_back(p.set_piece); });
    partitions.emplace_back();
    for_each(partition_b_copy.begin(), partition_b_copy.end(), [&partitions](auto p) { partitions.back().push_back(p.set_piece); });
    sanity_check(graph, partitions, 2);
    partitions.clear();

    while (gain > 0) {
        partition_a = partition_a_copy;
        partition_b = partition_b_copy;

        gain = sbg_partitioner::using_cc::run_bisection(graph, cost_matrix, partition_a_copy, partition_b_copy, cc_map, 0, 0);

        partitions.emplace_back();
        for_each(partition_a_copy.begin(), partition_a_copy.end(), [&partitions](auto p) { partitions.back().push_back(p.set_piece); });
        partitions.emplace_back();
        for_each(partition_b_copy.begin(), partition_b_copy.end(), [&partitions](auto p) { partitions.back().push_back(p.set_piece); });
        sanity_check(graph, partitions, 2);
        partitions.clear();

        cout << "gain is " << gain << endl;
    }
}


}

}
