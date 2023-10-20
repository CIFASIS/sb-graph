/** @file eval_natbt.hpp

 @brief <b>Natural base type evaluator</b>

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

#ifndef AST_VISITOR_EVAL_NATBT
#define AST_VISITOR_EVAL_NATBT

#include "eval/defs.hpp"

namespace SBG {

namespace Eval {

struct EvalNatBT : public boost::static_visitor<NatBaseType> {
  public:
  NatBaseType operator()(Util::NAT v) const;
  NatBaseType operator()(Util::MD_NAT v) const;
  NatBaseType operator()(Util::RATIONAL v) const;
  NatBaseType operator()(LIB::Interval v) const;
  NatBaseType operator()(LIB::SetPiece v) const;
  NatBaseType operator()(LIB::UnordSet v) const;
  NatBaseType operator()(LIB::OrdSet v) const;
  NatBaseType operator()(LIB::LExp v) const;
  NatBaseType operator()(LIB::Exp v) const;
  NatBaseType operator()(LIB::BaseMap v) const;
  NatBaseType operator()(LIB::CanonMap v) const;
  NatBaseType operator()(LIB::BasePWMap v) const;
  NatBaseType operator()(LIB::CanonPWMap v) const;
  NatBaseType operator()(LIB::BaseSBG v) const;
  NatBaseType operator()(LIB::CanonSBG v) const;
  NatBaseType operator()(LIB::BaseDSBG v) const;
  NatBaseType operator()(LIB::CanonDSBG v) const;
};

} // namespace Eval

} // namespace SBG

#endif
