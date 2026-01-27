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

#pragma once

#include <map>
#include <iostream>

#include <sbg/sbg.hpp>
#include <sbg/set.hpp>

namespace SBG {

namespace LIB {

struct setCompare {
    bool operator()(const Set& a, const Set& b) const {
        return a.maxElem() < b.minElem();
    }
};

using EdgeCost = std::map<Set, unsigned, setCompare>;

using NodeWeight = std::map<Set, int, setCompare>;

class WeightedSBGraph : public SBG
{
public:
    WeightedSBGraph(WeightedSBGraph& graph);

    WeightedSBGraph(WeightedSBGraph&& graph);

    WeightedSBGraph(const Set &V, const PWMap &Vmap
        , const PWMap &map1, const PWMap &map2
        , const PWMap &Emap, const PWMap &subEmap);


    WeightedSBGraph(const WeightedSBGraph&) = delete;
    WeightedSBGraph operator=(WeightedSBGraph&& other) = delete;

    ~WeightedSBGraph();

    void set_node_weights(NodeWeight& node_weights);

    NodeWeight get_node_weights() const;

    void set_node_weight(const Set& node_set, int weight);

    int get_node_weight(const Set& node_set) const;


    void set_edge_costs(EdgeCost& edge_costs);

    EdgeCost get_edge_costs() const;

    void set_edge_cost(const Set& edge_set, unsigned cost);

    unsigned get_edge_cost(const Set& edge_set) const;

private:
    NodeWeight _node_weights;

    EdgeCost _edge_costs;
};

std::ostream& operator<<(std::ostream& os, const WeightedSBGraph& graph);

}

}
