/** @file eval_graph.hpp

 @brief <b>Graph base type evaluator</b>

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

#ifndef AST_VISITOR_EVAL_GRAPH
#define AST_VISITOR_EVAL_GRAPH

#include "eval/defs.hpp"

namespace SBG {

namespace Eval {

struct EvalGraph : public boost::static_visitor<SBGBaseType> {
  public:
  SBGBaseType operator()(Util::NAT v) const;
  SBGBaseType operator()(Util::MD_NAT v) const;
  SBGBaseType operator()(Util::RATIONAL v) const;
  SBGBaseType operator()(LIB::Interval v) const;
  SBGBaseType operator()(LIB::SetPiece v) const;
  SBGBaseType operator()(LIB::UnordSet v) const;
  SBGBaseType operator()(LIB::OrdSet v) const;
  SBGBaseType operator()(LIB::LExp v) const;
  SBGBaseType operator()(LIB::Exp v) const;
  SBGBaseType operator()(LIB::BaseMap v) const;
  SBGBaseType operator()(LIB::CanonMap v) const;
  SBGBaseType operator()(LIB::BasePWMap v) const;
  SBGBaseType operator()(LIB::CanonPWMap v) const;
  SBGBaseType operator()(LIB::BaseSBG v) const;
  SBGBaseType operator()(LIB::CanonSBG v) const;
  SBGBaseType operator()(LIB::BaseDSBG v) const;
  SBGBaseType operator()(LIB::CanonDSBG v) const;
};

} // namespace Eval

} // namespace SBG

#endif
