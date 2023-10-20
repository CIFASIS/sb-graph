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

#include "eval/visitors/eval_map.hpp"

namespace SBG {

namespace Eval {

MapBaseType EvalMap::operator()(Util::NAT v) const
{
  Util::ERROR("EvalMap: trying to evaluate a NAT");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(Util::MD_NAT v) const 
{
  Util::ERROR("EvalMap: trying to evaluate a MD_NAT");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(Util::RATIONAL v) const 
{ 
  Util::ERROR("EvalMap: trying to evaluate a RATIONAL");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::Interval v) const
{
  Util::ERROR("EvalMap: trying to evaluate an Interval");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::SetPiece v) const
{
  Util::ERROR("EvalMap: trying to evaluate a SetPiece");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::UnordSet v) const
{
  Util::ERROR("EvalMap: trying to evaluate an UnordSet");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::OrdSet v) const
{
  Util::ERROR("EvalMap: trying to evaluate an OrdSet");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::LExp v) const 
{
  Util::ERROR("EvalMap: trying to evaluate a LExp");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::Exp v) const
{
  Util::ERROR("EvalMap: trying to evaluate an Exp");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::BaseMap v) const { return MapBaseType(v); }

MapBaseType EvalMap::operator()(LIB::CanonMap v) const { return MapBaseType(v); }

MapBaseType EvalMap::operator()(LIB::BasePWMap v) const { return MapBaseType(v); }

MapBaseType EvalMap::operator()(LIB::CanonPWMap v) const { return MapBaseType(v); }

MapBaseType EvalMap::operator()(LIB::BaseSBG v) const
{
  Util::ERROR("EvalMap: trying to evaluate a a BaseSBG");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::CanonSBG v) const
{
  Util::ERROR("EvalMap: trying to evaluate a CanonSBG");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::BaseDSBG v) const
{
  Util::ERROR("EvalMap: trying to evaluate a a BaseDSBG");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LIB::CanonDSBG v) const
{
  Util::ERROR("EvalMap: trying to evaluate a CanonDSBG");
  return MapBaseType();
}

} // namespace Eval

} // namespace SBG
