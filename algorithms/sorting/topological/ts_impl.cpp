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

#include "algorithms/sorting/topological/ts_impl.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Topological Sorting implementations
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const TSKind kind)
{
  switch (kind) {
    case TSKind::kMinVertex: {
      out << "minimum vertex";
      break;
    }

    default: {
      Util::ERROR("TSKind::operator<<: unsupported topological sorting "
        , "algorithm implementation");
      break;
    }
  }

  return out;
}

TSImplementation::TSImplementation() : _kind(TSKind::kMinVertex) {}

TSImplementation& TSImplementation::instance()
{
  static TSImplementation _instance;
  return _instance;
}

const TSKind& TSImplementation::kind() const { return _kind; }

void TSImplementation::set_ts_fact(TSKind kind) { _kind = kind; }

} // namespace LIB

} // namespace SBG
