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

#include "algorithms/tearing/tearing_impl.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Tearing implementations -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const TearingKind kind)
{
  switch (kind) {
    case TearingKind::kTearingV1: {
      out << "Tearing V1";
      break;
    }

    default: {
      Util::ERROR("Unsupported Tearing implementation");
      break;
    }
  }

  return out;
}

TearingImplementation::TearingImplementation() : _kind(TearingKind::kTearingV1) {}

TearingImplementation& TearingImplementation::instance()
{
  static TearingImplementation _instance;
  return _instance;
}

const TearingKind& TearingImplementation::kind() const { return _kind; }

void TearingImplementation::set_tearing_fact(TearingKind kind) { _kind = kind; }

} // namespace LIB

} // namespace SBG
