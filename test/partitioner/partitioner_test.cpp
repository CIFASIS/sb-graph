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

TEST(create_sb_grpah, PartitionerTests)
{
  // create needed factories
  SBG::LIB::UnordAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);

  auto sb_graph = sbg_partitioner::build_sb_graph("data/air_conditioners_1000.json", set_fact, map_fact, pw_fact);

  // create nodes of the expected graph
  auto expected_nodes = set_fact.createSet();
  expected_nodes.emplaceBack(Interval(0, 1, 999));
  expected_nodes.emplaceBack(Interval(1000, 1, 1999));
  expected_nodes.emplaceBack(Interval(2000, 1, 2999));
  expected_nodes.emplaceBack(Interval(3000, 1, 3999));
  EXPECT_EQ(expected_nodes, sb_graph.V());

  // create edges of the expected graph through its maps
  PWMap lhs_maps = pw_fact.createPWMap();
  PWMap rhs_maps = pw_fact.createPWMap();

  lhs_maps.emplaceBack(map_fact.createMap(Interval(4000, 1, 4999), Exp(LExp(1, RATIONAL(-3000, 1)))));
  rhs_maps.emplaceBack(map_fact.createMap(Interval(4000, 1, 4999), Exp(LExp(1, RATIONAL(-4000, 1)))));

  lhs_maps.emplaceBack(map_fact.createMap(Interval(5000, 1, 5999), Exp(LExp(1, RATIONAL(-2000, 1)))));
  rhs_maps.emplaceBack(map_fact.createMap(Interval(5000, 1, 5999), Exp(LExp(1, RATIONAL(-5000, 1)))));

  lhs_maps.emplaceBack(map_fact.createMap(Interval(6000, 1, 6999), Exp(LExp(1, RATIONAL(-6000, 1)))));
  rhs_maps.emplaceBack(map_fact.createMap(Interval(6000, 1, 6999), Exp(LExp(1, RATIONAL(-5000, 1)))));

  lhs_maps.emplaceBack(map_fact.createMap(Interval(7000, 1, 7999), Exp(LExp(1, RATIONAL(-5000, 1)))));
  rhs_maps.emplaceBack(map_fact.createMap(Interval(7000, 1, 7999), Exp(LExp(1, RATIONAL(-6000, 1)))));

  // test edges are correct
  EXPECT_EQ(lhs_maps.dom(), sb_graph.E());
  EXPECT_EQ(rhs_maps.dom(), sb_graph.E());

  // test that maps are as expected
  EXPECT_EQ(lhs_maps, sb_graph.map1());
  EXPECT_EQ(rhs_maps, sb_graph.map2());
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