/*****************************************************************************

    This file is part of SBG Library.

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

#include <cstdlib>
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

#include <algorithms/partitioner/build_sb_graph.hpp>
#include <algorithms/partitioner/communication_cost.hpp>
#include <algorithms/partitioner/kernighan_lin_partitioner.hpp>
#include <algorithms/partitioner/partition_graph.hpp>

/// @file parser_test.cpp
///

using namespace SBG::LIB;
using SBG::LIB::detail::Interval;
using SBG::LIB::detail::MultiDimInter;
using namespace sbg_partitioner;

/// @addtogroup SBGraphParserTests
/// @{
class PartitionerTests : public testing::TestWithParam<const char*> {};

static const std::string PARTITIONER_DATA_DIR = "../../partitioner/data/";

const std::string get_full_file_name(const std::string json_file)
{
  const std::string FULL_FILE_NAME = PARTITIONER_DATA_DIR + json_file;
  return FULL_FILE_NAME;
}

/**
 * @brief It creates a set based graph out of `filename` and compare its content
 * with the arguments. The purpose of this function is mainly to avoid code
 * repetition while testing the creation of different sb graphs.
 *
 * @param filename the algorithm input, used to build the sb graph.
 * @param node_intervals the domain intervals.
 * @param lhs_map_domain lhs map domain.
 * @param lhs_map_exps lhs map expresions.
 * @param rhs_map_domain rhs map domain.
 * @param rhs_map_exps rhs map expresions.
 */
static void test_create_sb_graph(const std::string& filename, const std::vector<Set>& node_intervals,
                                 const std::vector<Set>& lhs_map_domain, const std::vector<Expression>& lhs_map_exps,
                                 const std::vector<Set>& rhs_map_domain, const std::vector<Expression>& rhs_map_exps)
{
  auto sb_graph = sbg_partitioner::build_sb_graph(filename);

  // create nodes of the expected graph
  auto expected_nodes = Set();
  for (const auto& n : node_intervals) {
    expected_nodes = expected_nodes.cup(n);
  }
  EXPECT_EQ(expected_nodes, sb_graph.V());

  // create edges of the expected graph through its maps
  PWMap lhs_maps = PWMap();
  for (size_t i = 0; i < lhs_map_domain.size(); i++) {
    lhs_maps.insert(Map(lhs_map_domain[i], lhs_map_exps[i]));
  }

  PWMap rhs_maps = PWMap();
  for (size_t i = 0; i < rhs_map_domain.size(); i++) {
    rhs_maps.insert(Map(rhs_map_domain[i], rhs_map_exps[i]));
  }

  // test edges are correct
  EXPECT_EQ(true, lhs_maps.domain() == sb_graph.E());
  EXPECT_EQ(true, rhs_maps.domain() == sb_graph.E());

  //// test that maps are as expected
  EXPECT_EQ(true, lhs_maps == sb_graph.map1());
  EXPECT_EQ(true, rhs_maps == sb_graph.map2());
}

TEST(create_sb_graph, PartitionerTests)
{
  test_create_sb_graph(
      get_full_file_name("air_conditioners_1000.json"), {Set(0, 1, 999), Set(1000, 1, 1999), Set(2000, 1, 2999), Set(3000, 1, 3999)},
      {Set(4000, 1, 4999), Set(5000, 1, 5999), Set(6000, 1, 6999), Set(7000, 1, 7999)},
      {Expression(Rational(1), -3000), Expression(Rational(1), -2000), Expression(Rational(1), -6000), Expression(Rational(1), -5000)},
      {Set(4000, 1, 4999), Set(5000, 1, 5999), Set(6000, 1, 6999), Set(7000, 1, 7999)},
      {Expression(Rational(1), -4000), Expression(Rational(1), -5000), Expression(Rational(1), -5000), Expression(Rational(1), -6000)});

  test_create_sb_graph(get_full_file_name("advection.json"), {Set(0, 1, 0), Set(1, 1, 99)}, {Set(100, 1, 100), Set(101, 1, 198)},
                       {Expression(Rational(0), 0), Expression(Rational(1), -100)}, {Set(100, 1, 100), Set(101, 1, 198)},
                       {Expression(Rational(1), -99), Expression(Rational(1), -99)});
}

TEST(initial_partition, PartitionerTests)
{
  auto sb_graph = sbg_partitioner::build_sb_graph(get_full_file_name("air_conditioners_1000.json"));

  constexpr bool enable_multithreading = false;
  sbg_partitioner::PartitionMap partition =
      sbg_partitioner::best_initial_partition(sb_graph, 4, sbg_partitioner::InitialPartitionStrategy::ALL, enable_multithreading);

  auto expected_distributed_pre_order_0 = Set();
  expected_distributed_pre_order_0 = expected_distributed_pre_order_0.cup(Set(0, 1, 249));
  expected_distributed_pre_order_0 = expected_distributed_pre_order_0.cup(Set(1000, 1, 1249));
  expected_distributed_pre_order_0 = expected_distributed_pre_order_0.cup(Set(2000, 1, 2249));
  expected_distributed_pre_order_0 = expected_distributed_pre_order_0.cup(Set(3000, 1, 3249));
  Set partition_0 = sbg_partitioner::from_vector(partition.at(0));
  EXPECT_EQ(expected_distributed_pre_order_0, partition_0);

  auto expected_distributed_pre_order_1 = Set();
  expected_distributed_pre_order_1 = expected_distributed_pre_order_1.cup(Set(250, 1, 499));
  expected_distributed_pre_order_1 = expected_distributed_pre_order_1.cup(Set(1250, 1, 1499));
  expected_distributed_pre_order_1 = expected_distributed_pre_order_1.cup(Set(2250, 1, 2499));
  expected_distributed_pre_order_1 = expected_distributed_pre_order_1.cup(Set(3250, 1, 3499));
  Set partition_1 = sbg_partitioner::from_vector(partition.at(1));
  EXPECT_EQ(expected_distributed_pre_order_1, partition_1);

  auto expected_distributed_pre_order_2 = Set();
  expected_distributed_pre_order_2 = expected_distributed_pre_order_2.cup(Set(500, 1, 749));
  expected_distributed_pre_order_2 = expected_distributed_pre_order_2.cup(Set(1500, 1, 1749));
  expected_distributed_pre_order_2 = expected_distributed_pre_order_2.cup(Set(2500, 1, 2749));
  expected_distributed_pre_order_2 = expected_distributed_pre_order_2.cup(Set(3500, 1, 3749));
  Set partition_2 = sbg_partitioner::from_vector(partition.at(2));
  EXPECT_EQ(expected_distributed_pre_order_2, partition_2);

  auto expected_distributed_pre_order_3 = Set();
  expected_distributed_pre_order_3 = expected_distributed_pre_order_3.cup(Set(750, 1, 999));
  expected_distributed_pre_order_3 = expected_distributed_pre_order_3.cup(Set(1750, 1, 1999));
  expected_distributed_pre_order_3 = expected_distributed_pre_order_3.cup(Set(2750, 1, 2999));
  expected_distributed_pre_order_3 = expected_distributed_pre_order_3.cup(Set(3750, 1, 3999));
  Set partition_3 = sbg_partitioner::from_vector(partition.at(3));
  EXPECT_EQ(expected_distributed_pre_order_3, partition_3);

  sbg_partitioner::get_communication_cost().clear_communication_cache();
}

static void test_partitioning(const std::string& filename, int number_of_partitions)
{
  auto sb_graph = sbg_partitioner::build_sb_graph(filename);
  constexpr bool enable_multithreading = false;
  auto partitions = sbg_partitioner::best_initial_partition(sb_graph, number_of_partitions, sbg_partitioner::InitialPartitionStrategy::ALL,
                                                            enable_multithreading);
  sbg_partitioner::kl_sbg_imbalance_partitioner(sb_graph, partitions, 0.0, enable_multithreading);

  sbg_partitioner::sanity_check(sb_graph, partitions, number_of_partitions);

  // clear cache after running it
  sbg_partitioner::get_communication_cost().clear_communication_cache();
}

TEST(partitioning, PartitionerTests)
{
  test_partitioning(get_full_file_name("advection.json"), 2);
  test_partitioning(get_full_file_name("advection.json"), 4);
  test_partitioning(get_full_file_name("advection.json"), 8);

  test_partitioning(get_full_file_name("air_conditioners_1000.json"), 2);
  test_partitioning(get_full_file_name("air_conditioners_1000.json"), 4);
  test_partitioning(get_full_file_name("air_conditioners_1000.json"), 8);

  test_partitioning(get_full_file_name("air_conditioners_cont_4_1000.json"), 4);
}

namespace {

Set make_set(const std::vector<std::vector<std::tuple<int, int, int>>>& pieces)
{
  Set s;
  for (const auto& piece : pieces) {
    MultiDimInter mdi;
    for (const auto& [lo, step, hi] : piece) {
      mdi.emplaceBack(Interval(lo, step, hi));
    }
    s = s.cup(sbg_partitioner::to_set(mdi));
  }
  return s;
}

// Compares sets by element content, not by piece layout, since cup/difference may fragment pieces.
void expect_same_set(const Set& actual, const Set& expected, const std::string& label)
{
  EXPECT_TRUE(actual.difference(expected).isEmpty()) << label << ": actual has elements not in expected: " << actual.difference(expected);
  EXPECT_TRUE(expected.difference(actual).isEmpty()) << label << ": expected has elements not in actual: " << expected.difference(actual);
}

void check_cut(const Set& input, size_t size, const Set& expected_taken, const Set& expected_remaining, const std::string& label)
{
  auto [taken, remaining] = cut_interval_by_dimension(input, NodeWeight(), size);
  expect_same_set(taken, expected_taken, label + " taken");
  expect_same_set(remaining, expected_remaining, label + " remaining");
  EXPECT_TRUE(taken.intersection(remaining).isEmpty()) << label << ": taken and remaining overlap";
  expect_same_set(taken.cup(remaining), input, label + " union");
  EXPECT_EQ(taken.cardinal(), expected_taken.cardinal()) << label << " taken cardinal";
  EXPECT_EQ(remaining.cardinal(), expected_remaining.cardinal()) << label << " remaining cardinal";
}

}  // namespace

TEST(cut_interval_by_dimension, PartitionerTests)
{
  // 1-D: take 4 from [0:9].
  check_cut(make_set({{{0, 1, 9}}}), 4, make_set({{{0, 1, 3}}}), make_set({{{4, 1, 9}}}), "1D");

  // 2-D: take 10 from [0:2]x[0:3] -> {[0:1]x[0:3], [2:2]x[0:1]}, remaining {[2:2]x[2:3]}.
  check_cut(make_set({{{0, 1, 2}, {0, 1, 3}}}), 10, make_set({{{0, 1, 1}, {0, 1, 3}}, {{2, 1, 2}, {0, 1, 1}}}),
            make_set({{{2, 1, 2}, {2, 1, 3}}}), "2D");

  // 3-D: take 17 from [0:1]x[0:2]x[0:3] (cardinal 24).
  check_cut(make_set({{{0, 1, 1}, {0, 1, 2}, {0, 1, 3}}}), 17,
            make_set({{{0, 1, 0}, {0, 1, 2}, {0, 1, 3}}, {{1, 1, 1}, {0, 1, 0}, {0, 1, 3}}, {{1, 1, 1}, {1, 1, 1}, {0, 1, 0}}}),
            make_set({{{1, 1, 1}, {1, 1, 1}, {1, 1, 3}}, {{1, 1, 1}, {2, 1, 2}, {0, 1, 3}}}), "3D");

  // Multi-piece 2-D (lex-ordered pieces): first full, second partially cut.
  check_cut(make_set({{{0, 1, 1}, {0, 1, 1}}, {{5, 1, 6}, {0, 1, 1}}}), 6, make_set({{{0, 1, 1}, {0, 1, 1}}, {{5, 1, 5}, {0, 1, 1}}}),
            make_set({{{6, 1, 6}, {0, 1, 1}}}), "multi-piece 2D");

  // Interleaved pieces (partial remainder sorts after a later untouched piece — exercises the sort before OrderedSet).
  check_cut(make_set({{{0, 1, 1}, {0, 1, 1}}, {{0, 1, 1}, {5, 1, 6}}}), 3, make_set({{{0, 1, 0}, {0, 1, 1}}, {{1, 1, 1}, {0, 1, 0}}}),
            make_set({{{0, 1, 1}, {5, 1, 6}}, {{1, 1, 1}, {1, 1, 1}}}), "interleaved pieces");

  // Edge: size == 0.
  Set input_edge = make_set({{{0, 1, 2}, {0, 1, 3}}});
  check_cut(input_edge, 0, Set(), input_edge, "size==0");

  // Edge: size >= cardinal.
  check_cut(input_edge, input_edge.cardinal(), input_edge, Set(), "size==cardinal");
  check_cut(input_edge, input_edge.cardinal() + 42, input_edge, Set(), "size>cardinal");
}

// TEST(DISABLED_test_adjacency_matrix, PartitionerTests)
// {
//     std::array<std::string, 3> files = { "air_conditioners_1000.json", "air_conditioners_cont_4_1000.json", "advection.json" };

//     for (const auto& f : files) {
//         // clear communication cache
//         sbg_partitioner::get_communication_cost().clear_communication_cache();

//         auto sb_graph = sbg_partitioner::build_sb_graph(get_full_file_name(f), false);

//         auto injective_conn = sbg_partitioner::split_sets_according_to_relations(sb_graph);

//         sbg_partitioner::using_cc::SetPointers sorted_nodes = {};
//         unsigned index = 0;
//         for (const auto& s : injective_conn) {
//             sorted_nodes.push_back(sbg_partitioner::using_cc::SetPointer(index++, s, 0, s.cardinal()));
//         }

//         sbg_partitioner::CommunicationCostCC comm_cc(sb_graph, sorted_nodes);

//         for (size_t i = 0; i < sorted_nodes.size(); i++) {
//             const auto& s1 = sorted_nodes.at(i);
//             for (size_t j = i + 1; j < sorted_nodes.size(); j++) {
//                 const auto& s2 = sorted_nodes.at(j);

//                 auto comm = comm_cc.get_communication(s1.index, s2.index);
//                 if (comm > 0) {
//                     EXPECT_EQ(s1.set_piece.cardinal(), s2.set_piece.cardinal());
//                 }
//             }
//         }

//         std::vector<sbg_partitioner::using_cc::SetPointers> sorted_partitions;
//         auto new_graph = SBG::LIB::WeightedSBGraph(
//             injective_conn,
//             sb_graph.Vmap(),
//             sb_graph.map1(),
//             sb_graph.map2(),
//             sb_graph.Emap(),
//             sb_graph.subEmap()
//         );

//         auto non_sorted_partitions = sbg_partitioner::best_initial_partition(new_graph, 2,
//         sbg_partitioner::InitialPartitionStrategy::ALL, false);

//         for (const auto& partition : non_sorted_partitions) {
//             sorted_partitions.emplace_back();
//             for (const auto& v : partition) {
//                 for (const auto& n : sorted_nodes) {
//                     if (not v.intersection(n.set_piece).isEmpty()) {
//                         size_t offset = v.begin()[0].begin() - n.set_piece.begin()[0].begin();
//                         sorted_partitions.back().emplace_back(n.index, n.set_piece, offset, v.cardinal());
//                     }
//                 }
//             }
//         }

//         auto comm = sbg_partitioner::CommunicationCost(new_graph, non_sorted_partitions);
//         for (size_t i = 0; i < sorted_partitions.size(); i++) {
//             const auto p1 = sorted_partitions.at(i);
//             for (const auto& s1 : p1) {
//                 auto common_edges = comm.get_ec_by_interval(i, s1.set_piece);
//                 common_edges = common_edges.cup(comm.get_ic_by_interval(i, s1.set_piece));
//                 auto comm_number = comm_cc.get_communication(s1.index);

//                 EXPECT_EQ(common_edges.cardinal(), comm_number);
//             }
//         }

//         for (size_t i = 0; i < sorted_partitions.size(); i++) {
//             const auto p1 = sorted_partitions.at(i);
//             for (size_t j = i + 1; j < sorted_partitions.size(); j++) {
//                 const auto p2 = sorted_partitions.at(j);
//                 for (const auto& s1 : p1) {
//                     for (const auto& s2 : p2) {
//                         if (s1.index == s2.index) { continue; }
//                         auto edges_1 = comm.get_set_piece_edges(s1.set_piece);
//                         auto edges_2 = comm.get_set_piece_edges(s2.set_piece);
//                         auto edges = edges_1.intersection(edges_2);
//                         auto comm_number = comm_cc.get_communication(s1.index, s2.index);

//                         EXPECT_EQ(edges.cardinal(), comm_number);
//                     }
//                 }
//             }
//         }
//     }
// }
/// @}
