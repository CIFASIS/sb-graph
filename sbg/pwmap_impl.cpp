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

#include "sbg/pwmap_impl.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// PWMap implementations -------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, PWMapKind kind)
{
  switch (kind) {
    case PWMapKind::kUnordered: {
      out << "unordered";
      break;
    }

    case PWMapKind::kOrdered: {
      out << "ordered";
      break;
    }

    case PWMapKind::kDomOrdered: {
      out << "domain ordered";
      break;
    }

    default: {
      Util::ERROR("Unsupported ", kind, " implementation\n");
      break;
    }
  }

  return out;
}

PWMapImplementation::PWMapImplementation() : _kind(PWMapKind::kUnordered) {}

PWMapImplementation& PWMapImplementation::instance()
{
  static PWMapImplementation _instance;
  return _instance;
}

const PWMapKind& PWMapImplementation::kind() const { return _kind; }

void PWMapImplementation::set_pwmap_fact(PWMapKind kind)
{
  _kind = kind;
}

} // namespace LIB

} // namespace SBG
