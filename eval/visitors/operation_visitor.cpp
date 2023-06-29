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

#include <eval/visitors/operation_visitor.hpp>

namespace SBG {

namespace Eval {

// Cardinality -----------------------------------------------------------------

CardinalVisitor::CardinalVisitor() {}

unsigned int CardinalVisitor::operator()(Util::INT v) const { 
  Util::ERROR("CardinalityVisitor: trying to calculate card of INT");
  return 0;
}

unsigned int CardinalVisitor::operator()(Util::RATIONAL v) const { 
  Util::ERROR("CardinalityVisitor: trying to calculate card of INT");
  return 0;
}

unsigned int CardinalVisitor::operator()(SBG::Interval i) const { 
  return cardinal(i);
}

// Membership ------------------------------------------------------------------

} // namespace Eval

} // namespace SBG
