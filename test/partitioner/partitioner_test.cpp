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
#include <algorithms/partitioner/partition_graph_cc.hpp>

/// @file parser_test.cpp
///

using namespace SBG::LIB;

/// @addtogroup SBGraphParserTests
/// @{
class PartitionerTests : public testing::TestWithParam<const char*> {
};

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
static void test_create_sb_graph(const std::string& filename, const std::vector<Interval>& node_intervals,
                                 const std::vector<Interval>& lhs_map_domain, const std::vector<Exp>& lhs_map_exps,
                                 const std::vector<Interval>& rhs_map_domain, const std::vector<Exp>& rhs_map_exps)
{
  auto sb_graph = sbg_partitioner::build_sb_graph(filename);

  // create nodes of the expected graph
  auto expected_nodes = SET_FACT.createSet();
  for (const auto& n : node_intervals) {
    expected_nodes.emplaceBack(n);
  }
  EXPECT_EQ(expected_nodes, sb_graph.V());

  // create edges of the expected graph through its maps
  PWMap lhs_maps = PW_FACT.createPWMap();
  for (size_t i = 0; i < lhs_map_domain.size(); i++) {
    lhs_maps.emplaceBack(Map(lhs_map_domain[i], lhs_map_exps[i]));
  }

  PWMap rhs_maps = PW_FACT.createPWMap();
  for (size_t i = 0; i < rhs_map_domain.size(); i++) {
    rhs_maps.emplaceBack(Map(rhs_map_domain[i], rhs_map_exps[i]));
  }

  // test edges are correct
  EXPECT_EQ(lhs_maps.dom(), sb_graph.E());
  EXPECT_EQ(rhs_maps.dom(), sb_graph.E());

  // test that maps are as expected
  EXPECT_EQ(lhs_maps, sb_graph.map1());
  EXPECT_EQ(rhs_maps, sb_graph.map2());
}

TEST(create_sb_graph, PartitionerTests)
{
  test_create_sb_graph(get_full_file_name("air_conditioners_1000.json"),
                       {Interval(0, 1, 999), Interval(1000, 1, 1999), Interval(2000, 1, 2999), Interval(3000, 1, 3999)},
                       {Interval(4000, 1, 4999), Interval(5000, 1, 5999), Interval(6000, 1, 6999), Interval(7000, 1, 7999)},
                       {Exp(LExp(1, RATIONAL(-3000, 1))), Exp(LExp(1, RATIONAL(-2000, 1))), Exp(LExp(1, RATIONAL(-6000, 1))),
                        Exp(LExp(1, RATIONAL(-5000, 1)))},
                       {Interval(4000, 1, 4999), Interval(5000, 1, 5999), Interval(6000, 1, 6999), Interval(7000, 1, 7999)},
                       {Exp(LExp(1, RATIONAL(-4000, 1))), Exp(LExp(1, RATIONAL(-5000, 1))), Exp(LExp(1, RATIONAL(-5000, 1))),
                        Exp(LExp(1, RATIONAL(-6000, 1)))});

  test_create_sb_graph(get_full_file_name("advection.json"), {Interval(0, 1, 0), Interval(1, 1, 99)},
                       {Interval(100, 1, 100), Interval(101, 1, 198)}, {Exp(LExp(0, 0)), Exp(LExp(1, RATIONAL(-100, 1)))},
                       {Interval(100, 1, 100), Interval(101, 1, 198)}, {Exp(LExp(1, RATIONAL(-99, 1))), Exp(LExp(1, RATIONAL(-99, 1)))});
}

TEST(initial_partition, PartitionerTests)
{
  auto sb_graph = sbg_partitioner::build_sb_graph(get_full_file_name("air_conditioners_1000.json"));

  constexpr bool enable_multithreading = false;
  sbg_partitioner::PartitionMap partition = sbg_partitioner::best_initial_partition(sb_graph, 4, sbg_partitioner::InitialPartitionStrategy::ALL, enable_multithreading);

  auto expected_distributed_pre_order_0 = SET_FACT.createSet();
  expected_distributed_pre_order_0.emplaceBack(Interval(0, 1, 249));
  expected_distributed_pre_order_0.emplaceBack(Interval(1000, 1, 1249));
  expected_distributed_pre_order_0.emplaceBack(Interval(2000, 1, 2249));
  expected_distributed_pre_order_0.emplaceBack(Interval(3000, 1, 3249));
  Set partition_0 = sbg_partitioner::from_vector(partition.at(0));
  EXPECT_EQ(expected_distributed_pre_order_0, partition_0);

  auto expected_distributed_pre_order_1 = SET_FACT.createSet();
  expected_distributed_pre_order_1.emplaceBack(Interval(250, 1, 499));
  expected_distributed_pre_order_1.emplaceBack(Interval(1250, 1, 1499));
  expected_distributed_pre_order_1.emplaceBack(Interval(2250, 1, 2499));
  expected_distributed_pre_order_1.emplaceBack(Interval(3250, 1, 3499));
  Set partition_1 = sbg_partitioner::from_vector(partition.at(1));
  EXPECT_EQ(expected_distributed_pre_order_1, partition_1);

  auto expected_distributed_pre_order_2 = SET_FACT.createSet();
  expected_distributed_pre_order_2.emplaceBack(Interval(500, 1, 749));
  expected_distributed_pre_order_2.emplaceBack(Interval(1500, 1, 1749));
  expected_distributed_pre_order_2.emplaceBack(Interval(2500, 1, 2749));
  expected_distributed_pre_order_2.emplaceBack(Interval(3500, 1, 3749));
  Set partition_2 = sbg_partitioner::from_vector(partition.at(2));
  EXPECT_EQ(expected_distributed_pre_order_2, partition_2);

  auto expected_distributed_pre_order_3 = SET_FACT.createSet();
  expected_distributed_pre_order_3.emplaceBack(Interval(750, 1, 999));
  expected_distributed_pre_order_3.emplaceBack(Interval(1750, 1, 1999));
  expected_distributed_pre_order_3.emplaceBack(Interval(2750, 1, 2999));
  expected_distributed_pre_order_3.emplaceBack(Interval(3750, 1, 3999));
  Set partition_3 = sbg_partitioner::from_vector(partition.at(3));
  EXPECT_EQ(expected_distributed_pre_order_3, partition_3);

  sbg_partitioner::get_communication_cost().clear_communication_cache();
}

static void test_partitioning(const std::string& filename, int number_of_partitions)
{
  auto sb_graph = sbg_partitioner::build_sb_graph(filename);
  constexpr bool enable_multithreading = false;
  auto partitions = sbg_partitioner::best_initial_partition(sb_graph, number_of_partitions, sbg_partitioner::InitialPartitionStrategy::ALL, enable_multithreading);
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

TEST(test_adjacency_matrix, PartitionerTests)
{
    std::array<std::string, 3> files = { "air_conditioners_1000.json", "air_conditioners_cont_4_1000.json", "advection.json" };

    for (const auto& f : files) {
        // clear communication cache
        sbg_partitioner::get_communication_cost().clear_communication_cache();

        auto sb_graph = sbg_partitioner::build_sb_graph(get_full_file_name(f), false);

        auto injective_conn = sbg_partitioner::using_cc::split_nodes_into_injective_domains(sb_graph);

        sbg_partitioner::using_cc::SetPointers sorted_nodes = {};
        unsigned index = 0;
        for (const auto& s : injective_conn) {
            sorted_nodes.push_back(sbg_partitioner::using_cc::SetPointer(index++, s, 0, s.cardinal()));
        }

        sbg_partitioner::CommunicationCostCC comm_cc(sb_graph, sorted_nodes);

        for (size_t i = 0; i < sorted_nodes.size(); i++) {
            const auto& s1 = sorted_nodes.at(i);
            for (size_t j = i + 1; j < sorted_nodes.size(); j++) {
                const auto& s2 = sorted_nodes.at(j);

                auto comm = comm_cc.get_communication(s1.index, s2.index);
                if (comm > 0) {
                    EXPECT_EQ(s1.set_piece.cardinal(), s2.set_piece.cardinal());
                }
            }
        }

        std::vector<sbg_partitioner::using_cc::SetPointers> sorted_partitions;
        auto new_graph = SBG::LIB::WeightedSBGraph(
            injective_conn,
            sb_graph.Vmap(),
            sb_graph.map1(),
            sb_graph.map2(),
            sb_graph.Emap(),
            sb_graph.subEmap()
        );

        auto non_sorted_partitions = sbg_partitioner::best_initial_partition(new_graph, 2, sbg_partitioner::InitialPartitionStrategy::ALL, false);

        for (const auto& partition : non_sorted_partitions) {
            sorted_partitions.emplace_back();
            for (const auto& v : partition) {
                for (const auto& n : sorted_nodes) {
                    if (not v.intersection(n.set_piece).isEmpty()) {
                        size_t offset = v.begin()[0].begin() - n.set_piece.begin()[0].begin();
                        sorted_partitions.back().emplace_back(n.index, n.set_piece, offset, v.cardinal());
                    }
                }
            }
        }

        auto comm = sbg_partitioner::CommunicationCost(new_graph, non_sorted_partitions);
        for (size_t i = 0; i < sorted_partitions.size(); i++) {
            const auto p1 = sorted_partitions.at(i);
            for (const auto& s1 : p1) {
                auto common_edges = comm.get_ec_by_interval(i, s1.set_piece);
                common_edges = common_edges.cup(comm.get_ic_by_interval(i, s1.set_piece));
                auto comm_number = comm_cc.get_communication(s1.index);

                EXPECT_EQ(common_edges.cardinal(), comm_number);
            }
        }

        for (size_t i = 0; i < sorted_partitions.size(); i++) {
            const auto p1 = sorted_partitions.at(i);
            for (size_t j = i + 1; j < sorted_partitions.size(); j++) {
                const auto p2 = sorted_partitions.at(j);
                for (const auto& s1 : p1) {
                    for (const auto& s2 : p2) {
                        if (s1.index == s2.index) { continue; }
                        auto edges_1 = comm.get_set_piece_edges(s1.set_piece);
                        auto edges_2 = comm.get_set_piece_edges(s2.set_piece);
                        auto edges = edges_1.intersection(edges_2);
                        auto comm_number = comm_cc.get_communication(s1.index, s2.index);

                        EXPECT_EQ(edges.cardinal(), comm_number);
                    }
                }
            }
        }
    }
}
/// @}
