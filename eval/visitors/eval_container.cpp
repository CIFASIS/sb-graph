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

ContainerBaseType EvalContainer::operator()(ContainerBaseType v) const
{
  return v;
}

ContainerBaseType EvalContainer::operator()(LinearBaseType v) const
{
  Util::ERROR("EvalContainer: trying to evaluate a LinearBaseType");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(MapBaseType v) const
{
  Util::ERROR("EvalContainer: trying to evaluate a MapBaseType");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(SBGBaseType v) const
{
  Util::ERROR("EvalContainer: trying to evaluate a SBGBaseType");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(InfoBaseType v) const
{
  Util::ERROR("EvalContainer: trying to evaluate an InfoBaseType");
  return ContainerBaseType();
}

} // namespace Eval

} // namespace SBG
