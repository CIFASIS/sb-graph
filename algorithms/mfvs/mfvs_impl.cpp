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

#include "algorithms/mfvs/mfvs_impl.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// MFVS implementations -------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const MFVSKind kind)
{
  switch (kind) {
    case MFVSKind::kGreedy: {
      out << "maximum degree greedy";
      break;
    }

    case MFVSKind::kSmallSV: {
      out << "smallest set-vertex";
      break;
    }

    default: {
      Util::ERROR("MFVSKind::operator<<: unsupported MFVS implementation");
      break;
    }
  }

  return out;
}

MFVSImplementation::MFVSImplementation() : _kind(MFVSKind::kSmallSV) {}

MFVSImplementation& MFVSImplementation::instance()
{
  static MFVSImplementation _instance;
  return _instance;
}

const MFVSKind& MFVSImplementation::kind() const { return _kind; }

void MFVSImplementation::set_mfvs_fact(MFVSKind kind) { _kind = kind; }

} // namespace LIB

} // namespace SBG
