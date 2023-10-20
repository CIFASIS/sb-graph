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

#include "eval/visitors/eval_graph.hpp"

namespace SBG {

namespace Eval {

SBGBaseType EvalGraph::operator()(Util::NAT v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a NAT");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(Util::MD_NAT v) const 
{
  Util::ERROR("EvalGraph: trying to evaluate a MD_NAT");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(Util::RATIONAL v) const 
{ 
  Util::ERROR("EvalGraph: trying to evaluate a RATIONAL");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::Interval v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an Interval");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::SetPiece v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a SetPiece");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::UnordSet v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an UnordSet");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::OrdSet v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an OrdSet");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::LExp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a LExp");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::Exp v) const
{ 
  Util::ERROR("EvalGraph: trying to evaluate an Exp");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::BaseMap v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a BaseMap");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::CanonMap v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a CanonMap");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::BasePWMap v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a BasePWMap");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::CanonPWMap v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a CanonPWMap");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LIB::BaseSBG v) const { return SBGBaseType(v); }

SBGBaseType EvalGraph::operator()(LIB::CanonSBG v) const { return SBGBaseType(v); }

SBGBaseType EvalGraph::operator()(LIB::BaseDSBG v) const { return SBGBaseType(v); }

SBGBaseType EvalGraph::operator()(LIB::CanonDSBG v) const { return SBGBaseType(v); }

} // namespace Eval

} // namespace SBG
