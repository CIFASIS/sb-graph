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

#include "eval/visitors/eval_container.hpp"

namespace SBG {

namespace Eval {

ContainerBaseType EvalContainer::operator()(Util::NAT v) const
{
  Util::ERROR("EvalContainer: trying to evaluate a NAT");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(Util::MD_NAT v) const 
{
  Util::ERROR("EvalContainer: trying to evaluate a MD_NAT");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(Util::RATIONAL v) const 
{ 
  Util::ERROR("EvalContainer: trying to evaluate a RATIONAL");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(LIB::Interval v) const { return ContainerBaseType(v); }

ContainerBaseType EvalContainer::operator()(LIB::SetPiece v) const { return ContainerBaseType(v); }

ContainerBaseType EvalContainer::operator()(LIB::UnordSet v) const { return ContainerBaseType(v); }

ContainerBaseType EvalContainer::operator()(LIB::OrdSet v) const { return ContainerBaseType(v); }

ContainerBaseType EvalContainer::operator()(LIB::LExp v) const 
{
  Util::ERROR("EvalContainer: trying to evaluate a LExp");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(LIB::Exp v) const
{
  Util::ERROR("EvalContainer: trying to evaluate an Exp");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(LIB::BaseMap v) const
{
  Util::ERROR("EvalContainer: trying to evaluate a BaseMap");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(LIB::CanonMap v) const
{
  Util::ERROR("EvalContainer: trying to evaluate a CanonMap");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(LIB::BasePWMap v) const
{
  Util::ERROR("EvalContainer: trying to evaluate a BasePWMap");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(LIB::CanonPWMap v) const
{
  Util::ERROR("EvalContainer: trying to evaluate a CanonPWMap");
  return ContainerBaseType();
}

} // namespace Eval

} // namespace SBG
