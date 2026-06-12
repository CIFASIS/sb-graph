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

 ******************************************************************************/

#include <vector>

#include <sbg/sbg.hpp>

#pragma once

namespace sbg_partitioner {


typedef std::vector<SBG::LIB::Set> Partition;

typedef std::vector<Partition> PartitionMap;

/// @brief Overloading operator `<<` to print `Partition` objects
std::ostream& operator<<(std::ostream& os, const Partition& partitions);

/// @brief Overloading operator `<<` to print `PartitionMap` objects
std::ostream& operator<<(std::ostream& os, const PartitionMap& partitions);


namespace using_cc {

struct SetPointer {
    unsigned index;
    SBG::LIB::SetPiece set_piece;
    size_t offset;
    size_t size;

    SetPointer(unsigned index, const SBG::LIB::SetPiece& set_piece, size_t offset, size_t size)
        : index(index),
        set_piece(set_piece),
        offset(offset),
        size(size)
    {}

    bool operator==(const SetPointer& other) { return index == other.index and offset == other.offset and size == other.size; }
};

using SetPointers = std::vector<SetPointer>;

/// @brief Overloading operator `<<` to print `SetPointer` objects
std::ostream& operator<<(std::ostream& os, const SetPointer& set_pointer);

/// @brief Overloading operator `<<` to print `SetPointers` objects
std::ostream& operator<<(std::ostream& os, const SetPointers& set_pointers);

}

}