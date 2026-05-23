/*******************************************************************************

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

#include "sbg/set_impl.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Set implementations ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const SetKind kind)
{
  switch (kind) {
    case SetKind::kUnordered: {
      out << "unordered";
      break;
    }

    case SetKind::kOrdered: {
      out << "ordered";
      break;
    }

    case SetKind::kOrdUnidimDense: {
      out << "uni-dimensional ordered dense";
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " Set implementation\n");
      break;
    }
  }

  return out;
}

SetImplementation::SetImplementation() : _kind(SetKind::kUnordered) {}

SetImplementation& SetImplementation::instance()
{
  static SetImplementation _instance;
  return _instance;
}

const SetKind& SetImplementation::kind() const { return _kind; }

void SetImplementation::set_set_fact(SetKind kind) { _kind = kind; }

} // namespace LIB

} // namespace SBG
