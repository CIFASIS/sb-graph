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

#include <optional>
#include <string>

namespace sbg_partitioner {


enum InitialPartitionStrategy {
    ALL = 0,
    DFS_DISTRIBUTIVE_PREORDER = 1,
    DFS_DISTRIBUTIVE_POSTORDER = 2,
    DFS_GREEDY_PREORDER = 3,
    DFS_GREEDY_POSTORDER =4
};


struct PartitionerParams
{
    std::optional<std::string> config_filename = std::nullopt;
    std::optional<std::string> filename = std::nullopt;
    std::optional<std::string> directory = std::nullopt;
    std::optional<unsigned> number_of_partitions = std::nullopt;
    std::optional<std::string> output_file;
    std::optional<std::string> output_sb_graph = std::nullopt;
    float epsilon = 0.0;
    InitialPartitionStrategy initial_partition_strategy = InitialPartitionStrategy::ALL;
    bool enable_multithreading = false;
    bool compute_metrics = false;
};

}