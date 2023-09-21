/** @file eval_map.hpp

 @brief <b>Map base type evaluator</b>

 <hr>

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

#ifndef AST_VISITOR_EVAL_MAP
#define AST_VISITOR_EVAL_MAP

#include "eval/defs.hpp"

namespace SBG {

namespace Eval {

struct EvalMap : public boost::static_visitor<MapBaseType> {
  public:
  MapBaseType operator()(Util::NAT v) const;
  MapBaseType operator()(Util::MD_NAT v) const;
  MapBaseType operator()(Util::RATIONAL v) const;
  MapBaseType operator()(LIB::Interval v) const;
  MapBaseType operator()(LIB::SetPiece v) const;
  MapBaseType operator()(LIB::UnordSet v) const;
  MapBaseType operator()(LIB::OrdSet v) const;
  MapBaseType operator()(LIB::LExp v) const;
  MapBaseType operator()(LIB::Exp v) const;
  MapBaseType operator()(LIB::BaseMap v) const;
  MapBaseType operator()(LIB::CanonMap v) const;
  MapBaseType operator()(LIB::BasePWMap v) const;
  MapBaseType operator()(LIB::CanonPWMap v) const;
};

} // namespace Eval

} // namespace SBG

#endif
