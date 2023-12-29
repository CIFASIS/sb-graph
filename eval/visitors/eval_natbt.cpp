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

#include "eval/visitors/eval_natbt.hpp"

namespace SBG {

namespace Eval {

NatBaseType EvalNatBT::operator()(Util::MD_NAT v) const { return v; }

NatBaseType EvalNatBT::operator()(Util::RATIONAL v) const 
{ 
  Util::ERROR("EvalNatBT: trying to evaluate a RATIONAL");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(ContainerBaseType v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a ContainerBaseType");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LinearBaseType v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a LinearBaseType");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(MapBaseType v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a MapBaseType");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(SBGBaseType v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a SBGBaseType");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(InfoBaseType v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate an InfoBaseType");
  return NatBaseType();
}

} // namespace Eval

} // namespace SBG
