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

            int s = min(partition_a.at(i).size, partition_b.at(j).size);

            float alpha_i = s / float(partition_a.at(i).size);
            float alpha_j = s / float(partition_b.at(j).size);

            float gain = D_i * alpha_i + D_j * alpha_j;
            gain -= 2 * cost_matrix_cc.get_communication(partition_a.at(i).index, partition_b.at(j).index) * min(alpha_i, alpha_j);

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
    cout << "calling run_bisection with\n" << partition_a << "\n" << partition_b << endl;
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
        a_v.emplace_back(max_gain->a.index, max_gain->a.set_piece, max_gain->size);
        b_v.emplace_back(max_gain->b.index, max_gain->b.set_piece, max_gain->size);
        if (par_sum > max_par_sum) {
            max_par_sum = par_sum;
            max_par_sum_set = make_pair(a_v, b_v);
        }

        partition_a_copy.erase(remove(partition_a_copy.begin(), partition_a_copy.end(), max_gain->a));
        if (max_gain->size < max_gain->a.size) {
            partition_a_copy.emplace_back(max_gain->a.index, max_gain->a.set_piece, max_gain->a.size - max_gain->size);
        }

        partition_b_copy.erase(remove(partition_b_copy.begin(), partition_b_copy.end(), max_gain->b));
        if (max_gain->size < max_gain->b.size) {
            partition_b_copy.emplace_back(max_gain->b.index, max_gain->b.set_piece, max_gain->b.size - max_gain->size);
        }

        if (not partition_a_copy.empty() and not partition_b_copy.empty()) {
            tie(max_gain, gm) = compute_gains(partition_a_copy, partition_b_copy, cost_matrix);
        }
    }

    if (max_par_sum > 0) {
        cout << "\n" << max_par_sum_set.first << "\n" << max_par_sum_set.second << endl;
        for (const auto& a : max_par_sum_set.first) {
            cout << "moving " << a << endl;
            auto it = find_if(partition_a.begin(), partition_a.end(), [&a](const auto& s) { return s.index == a.index; } );
            assert(it != partition_a.end());
            cout << "found " << *it << endl;
            if (it->size == a.size) {
                partition_a.erase(it);
            } else {
                cout << "sizes " << it->size << ", " << a.size << endl;
                it->size -= a.size;
                cout << "new size " << it->size << endl;
                cout << partition_a << endl;
            }
        }

        for (const auto& b : max_par_sum_set.second) {
            cout << "moving " << b << endl;
            auto it = find_if(partition_b.begin(), partition_b.end(), [&b](const auto& s) { return s.index == b.index; } );
            cout << "found " << *it << endl;
            assert(it != partition_b.end());
            if (it->size == b.size) {
                partition_b.erase(it);
            } else {
                cout << "sizes " << it->size << ", " << b.size << endl;
                it->size -= b.size;
                cout << "new size " << it->size << endl;
            }
        }

        for (const auto& p : max_par_sum_set.second) {
            cout << Interval(p.set_piece.begin()[0].begin(), 1, p.set_piece.begin()[0].begin() + p.size - 1)<< " " << p.size << endl;
            partition_a.emplace_back(p.index, p.set_piece, p.size);
        }
        cout << endl;

        for (const auto& p : max_par_sum_set.first) {
            assert(p.size > 0);
            cout << Interval(p.set_piece.begin()[0].end() - p.size + 1, 1, p.set_piece.begin()[0].end()) << " " << p.size << endl;
            partition_b.emplace_back(p.index, p.set_piece, p.size);
        }
    }

    cout << "result:\nmax par sum: " << max_par_sum << "\npartition a: " << partition_a << "\npartition b: " << partition_b << endl;

    return max_par_sum;
}


void bisection(
    const SBG::LIB::WeightedSBGraph& graph,
    CommunicationCostCC& cost_matrix,
    const SetPointers& sorted_nodes,
    SetPointers& partition_a,
    SetPointers& partition_b,
    const PWMap& cc_map,
    unsigned LMin,
    unsigned LMax)
{
    auto partition_a_copy = partition_a;
    auto partition_b_copy = partition_b;

    int gain = sbg_partitioner::using_cc::run_bisection(graph, cost_matrix, partition_a_copy, partition_b_copy, cc_map, 0, 0);

    PartitionMap partitions = rebuild_partitions(sorted_nodes, { partition_a_copy, partition_b_copy });
    sanity_check(graph, partitions, 2);
    partitions.clear();

    while (gain > 0) {
        partition_a = partition_a_copy;
        partition_b = partition_b_copy;

        gain = sbg_partitioner::using_cc::run_bisection(graph, cost_matrix, partition_a_copy, partition_b_copy, cc_map, 0, 0);

        partitions = rebuild_partitions(sorted_nodes, { partition_a_copy, partition_b_copy });
        sanity_check(graph, partitions, 2);
        partitions.clear();

        cout << "gain is " << gain << endl;
    }
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

}

}
