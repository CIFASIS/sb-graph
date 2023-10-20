/** @file eval_container.hpp

 @brief <b>Container base type evaluator</b>

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

#ifndef AST_VISITOR_EVAL_CONTAINER
#define AST_VISITOR_EVAL_CONTAINER

#include <boost/variant/static_visitor.hpp>

#include "eval/defs.hpp"

namespace SBG {

namespace Eval {

struct EvalContainer : public boost::static_visitor<ContainerBaseType> {
  public:
  ContainerBaseType operator()(Util::NAT v) const;
  ContainerBaseType operator()(Util::MD_NAT v) const;
  ContainerBaseType operator()(Util::RATIONAL v) const;
  ContainerBaseType operator()(LIB::Interval v) const;
  ContainerBaseType operator()(LIB::SetPiece v) const;
  ContainerBaseType operator()(LIB::UnordSet v) const;
  ContainerBaseType operator()(LIB::OrdSet v) const;
  ContainerBaseType operator()(LIB::LExp v) const;
  ContainerBaseType operator()(LIB::Exp v) const;
  ContainerBaseType operator()(LIB::BaseMap v) const;
  ContainerBaseType operator()(LIB::CanonMap v) const;
  ContainerBaseType operator()(LIB::BasePWMap v) const;
  ContainerBaseType operator()(LIB::CanonPWMap v) const;
  ContainerBaseType operator()(LIB::BaseSBG v) const;
  ContainerBaseType operator()(LIB::CanonSBG v) const;
  ContainerBaseType operator()(LIB::BaseDSBG v) const;
  ContainerBaseType operator()(LIB::CanonDSBG v) const;
};

} // namespace Eval

} // namespace SBG

#endif
