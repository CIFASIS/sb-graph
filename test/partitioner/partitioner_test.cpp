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
#include <algorithms/partitioner/partition_graph.hpp>
#include <sbg/af_map.hpp>
#include <sbg/af_set.hpp>

/// @file parser_test.cpp
///

using namespace SBG::LIB;

/// @addtogroup SBGraphParserTests
/// @{
class PartitionerTests : public testing::TestWithParam<const char*> {
};


static void test_create_sb_graph(
    const std::string& filename,
    const std::vector<Interval>& node_intervals,
    const std::vector<Interval>& lhs_map_domain,
    const std::vector<Exp>& lhs_map_exps,
    const std::vector<Interval>& rhs_map_domain,
    const std::vector<Exp>& rhs_map_exps)
{
    // create needed factories
    SBG::LIB::UnordAF set_fact;
    SBG::LIB::MapAF map_fact(set_fact);
    SBG::LIB::UnordPWMapAF pw_fact(map_fact);

    auto sb_graph = sbg_partitioner::build_sb_graph(filename, set_fact, map_fact, pw_fact);

    // create nodes of the expected graph
    auto expected_nodes = set_fact.createSet();
    for (const auto& n : node_intervals) {
        expected_nodes.emplaceBack(n);
    }
    EXPECT_EQ(expected_nodes, sb_graph.V());

    // create edges of the expected graph through its maps
    PWMap lhs_maps = pw_fact.createPWMap();
    for (size_t i = 0; i < lhs_map_domain.size(); i++) {
       lhs_maps.emplaceBack(map_fact.createMap(lhs_map_domain[i], lhs_map_exps[i]));
    }

    PWMap rhs_maps = pw_fact.createPWMap();
    for (size_t i = 0; i < rhs_map_domain.size(); i++) {
       rhs_maps.emplaceBack(map_fact.createMap(rhs_map_domain[i], rhs_map_exps[i]));
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
    test_create_sb_graph(
        "data/air_conditioners_1000.json",
        { Interval(0, 1, 999), Interval(1000, 1, 1999), Interval(2000, 1, 2999), Interval(3000, 1, 3999) },
        { Interval(4000, 1, 4999), Interval(5000, 1, 5999), Interval(6000, 1, 6999), Interval(7000, 1, 7999) },
        { Exp(LExp(1, RATIONAL(-3000, 1))), Exp(LExp(1, RATIONAL(-2000, 1))), Exp(LExp(1, RATIONAL(-6000, 1))), Exp(LExp(1, RATIONAL(-5000, 1))) },
        { Interval(4000, 1, 4999), Interval(5000, 1, 5999), Interval(6000, 1, 6999), Interval(7000, 1, 7999) },
        { Exp(LExp(1, RATIONAL(-4000, 1))), Exp(LExp(1, RATIONAL(-5000, 1))), Exp(LExp(1, RATIONAL(-5000, 1))), Exp(LExp(1, RATIONAL(-6000, 1))) }
    );

    test_create_sb_graph(
        "data/advection.json",
        { Interval(0, 1, 0), Interval(1, 1, 99) },
        { Interval(100, 1, 100), Interval(101, 1, 198) },
        { Exp(LExp(0, 0)),  Exp(LExp(1, RATIONAL(-100, 1))) },
        { Interval(100, 1, 100), Interval(101, 1, 198) },
        { Exp(LExp(1, RATIONAL(-99, 1))), Exp(LExp(1, RATIONAL(-99, 1))) }
    );
}

TEST(initial_partition, PartitionerTests)
{
  // create needed factories
  SBG::LIB::UnordAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);

  auto sb_graph = sbg_partitioner::build_sb_graph("data/air_conditioners_1000.json", set_fact, map_fact, pw_fact);

  sbg_partitioner::PartitionMap partition = sbg_partitioner::best_initial_partition(sb_graph, 4, set_fact);

  auto expected_distributed_pre_order_0 = set_fact.createSet();
  expected_distributed_pre_order_0.emplaceBack(Interval(0, 1, 249));
  expected_distributed_pre_order_0.emplaceBack(Interval(1000, 1, 1249));
  expected_distributed_pre_order_0.emplaceBack(Interval(2000, 1, 2249));
  expected_distributed_pre_order_0.emplaceBack(Interval(3000, 1, 3249));
  EXPECT_EQ(expected_distributed_pre_order_0, partition.at(0));

  auto expected_distributed_pre_order_1 = set_fact.createSet();
  expected_distributed_pre_order_1.emplaceBack(Interval(250, 1, 499));
  expected_distributed_pre_order_1.emplaceBack(Interval(1250, 1, 1499));
  expected_distributed_pre_order_1.emplaceBack(Interval(2250, 1, 2499));
  expected_distributed_pre_order_1.emplaceBack(Interval(3250, 1, 3499));
  EXPECT_EQ(expected_distributed_pre_order_1, partition.at(1));

  auto expected_distributed_pre_order_2 = set_fact.createSet();
  expected_distributed_pre_order_2.emplaceBack(Interval(500, 1, 749));
  expected_distributed_pre_order_2.emplaceBack(Interval(1500, 1, 1749));
  expected_distributed_pre_order_2.emplaceBack(Interval(2500, 1, 2749));
  expected_distributed_pre_order_2.emplaceBack(Interval(3500, 1, 3749));
  EXPECT_EQ(expected_distributed_pre_order_2, partition.at(2));

  auto expected_distributed_pre_order_3 = set_fact.createSet();
  expected_distributed_pre_order_3.emplaceBack(Interval(750, 1, 999));
  expected_distributed_pre_order_3.emplaceBack(Interval(1750, 1, 1999));
  expected_distributed_pre_order_3.emplaceBack(Interval(2750, 1, 2999));
  expected_distributed_pre_order_3.emplaceBack(Interval(3750, 1, 3999));
  EXPECT_EQ(expected_distributed_pre_order_3, partition.at(3));
}
/// @}