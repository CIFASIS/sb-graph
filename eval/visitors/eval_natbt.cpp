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

NatBaseType EvalNatBT::operator()(Util::NAT v) const { return NatBaseType(v); }

NatBaseType EvalNatBT::operator()(Util::MD_NAT v) const { return NatBaseType(v); }

NatBaseType EvalNatBT::operator()(Util::RATIONAL v) const 
{ 
  Util::ERROR("EvalNatBT: trying to evaluate a RATIONAL");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::Interval v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate an Interval");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::SetPiece v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a SetPiece");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::UnordSet v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate an UnordSet");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::OrdSet v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate an OrdSet");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::LExp v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a LExp");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::Exp v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate an Exp");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::BaseMap v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a BaseMap");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::CanonMap v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a CanonMap");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::BasePWMap v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a BasePWMap");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::CanonPWMap v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a CanonPWMap");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::BaseSBG v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a BaseSBG");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::CanonSBG v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a CanonSBG");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::BaseDSBG v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a BaseDSBG");
  return NatBaseType();
}

NatBaseType EvalNatBT::operator()(LIB::CanonDSBG v) const
{
  Util::ERROR("EvalNatBT: trying to evaluate a CanonDSBG");
  return NatBaseType();
}

} // namespace Eval

} // namespace SBG
