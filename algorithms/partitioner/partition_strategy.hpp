/*****************************************************************************

 This file is part of SBG Partitioner.

 SBG Partitioner is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Partitioner is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Partitioner.  If not, see <http://www.gnu.org/licenses/>.
()
 ******************************************************************************/

#pragma once

#include <iostream>
#include <map>
#include <utility>
#include <stdlib.h>
#include <set>

#include "sbg/sbg.hpp"
#include "weighted_sb_graph.hpp"


namespace sbg_partitioner {


class PartitionStrategy
{
public:
    PartitionStrategy() = default;

    virtual ~PartitionStrategy() = default;

    virtual void operator() (const SBG::LIB::Set& node) = 0;

    virtual std::map<unsigned, std::set<SBG::LIB::Set, SBG::LIB::setCompare>> partitions() const = 0;
};

class PartitionStrategyGreedy : public PartitionStrategy
{
public:
    PartitionStrategyGreedy(unsigned number_of_partitions, const SBG::LIB::WeightedSBGraph graph);

    ~PartitionStrategyGreedy() = default;

    void operator() (const SBG::LIB::Set& node);

    std::map<unsigned, std::set<SBG::LIB::Set, SBG::LIB::setCompare>> partitions() const;

private:
    unsigned _number_of_partitions;
    unsigned _current_partition;
    unsigned _total_of_nodes;
    unsigned _acceptable_surplus;
    unsigned _acceptable_amount;
    std::map<unsigned, std::set<SBG::LIB::Set, SBG::LIB::setCompare>> _partitions;
    size_t _expected_size_by_partition;
    std::map<unsigned, size_t> _current_size_by_partition;
    SBG::LIB::NodeWeight _node_weight;
};


struct SizeCmp
{
    bool operator () (std::pair<unsigned, unsigned> a, std::pair<unsigned, unsigned> b) const
    {
        return std::get<1>(a) < std::get<1>(b);
    }
};

class PartitionStrategyDistributive : public PartitionStrategy
{
public:
    PartitionStrategyDistributive(unsigned number_of_partitions, const SBG::LIB::WeightedSBGraph graph);

    ~PartitionStrategyDistributive() = default;

    void operator() (const SBG::LIB::Set& node);

    std::map<unsigned, std::set<SBG::LIB::Set, SBG::LIB::setCompare>> partitions() const;

private:
    unsigned _number_of_partitions;
    unsigned _current_partition;
    size_t _total_of_nodes;
    size_t _acceptable_surplus;
    size_t _acceptable_amount;
    std::map<unsigned, std::set<SBG::LIB::Set, SBG::LIB::setCompare>> _partitions;
    std::map<size_t, size_t> _current_size_by_partition;
    SBG::LIB::Set _nodes;
    SBG::LIB::NodeWeight _node_weight;
};

std::ostream& operator<<(std::ostream& os, const PartitionStrategy& pgraph);

}
