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

#include "eval/visitors/eval_linear.hpp"

namespace SBG {

namespace Eval {

LinearBaseType EvalLinear::operator()(Util::NAT v) const
{
  Util::ERROR("EvalLinear: trying to evaluate a NAT");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(Util::MD_NAT v) const 
{
  Util::ERROR("EvalLinear: trying to evaluate a MD_NAT");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(Util::RATIONAL v) const 
{ 
  Util::ERROR("EvalLinear: trying to evaluate a RATIONAL");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(LIB::Interval v) const
{
  Util::ERROR("EvalLinear: trying to evaluate an Interval");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(LIB::SetPiece v) const
{
  Util::ERROR("EvalLinear: trying to evaluate a SetPiece");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(LIB::UnordSet v) const
{
  Util::ERROR("EvalLinear: trying to evaluate an UnordSet");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(LIB::OrdSet v) const
{
  Util::ERROR("EvalLinear: trying to evaluate an OrdSet");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(LIB::LExp v) const { return LinearBaseType(v); }

LinearBaseType EvalLinear::operator()(LIB::Exp v) const { return LinearBaseType(v); }

LinearBaseType EvalLinear::operator()(LIB::BaseMap v) const
{
  Util::ERROR("EvalLinear: trying to evaluate a BaseMap");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(LIB::CanonMap v) const
{
  Util::ERROR("EvalLinear: trying to evaluate a CanonMap");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(LIB::BasePWMap v) const
{
  Util::ERROR("EvalLinear: trying to evaluate a BasePWMap");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(LIB::CanonPWMap v) const
{
  Util::ERROR("EvalLinear: trying to evaluate a CanonPWMap");
  return LinearBaseType();
}

} // namespace Eval

} // namespace SBG
