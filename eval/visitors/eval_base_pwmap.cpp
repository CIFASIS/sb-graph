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

#include "eval/visitors/eval_base_pwmap.hpp"

namespace SBG {

namespace Eval {

EvalBasePWMap::EvalBasePWMap() : env_() {}
EvalBasePWMap::EvalBasePWMap(VarEnv env) : env_(env) {}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Natural v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate Natural ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate MDNatural ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Rational v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate Rational ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate Boolean ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<MapBaseType>(value)) {
      auto m = std::get<MapBaseType>(value);
      if (std::holds_alternative<LIB::BasePWMap>(m))
        return std::get<LIB::BasePWMap>(m);
    }

    else {
      Util::ERROR("EvalBasePWMap: variable ", v, " is not a PWMap\n");
      return LIB::BasePWMap(); 
    } 
  }

  Util::ERROR("EvalBasePWMap: variable ", v, " undefined\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Call v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate Call ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalBasePWMap: trying to evaluate Interval ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate InterBinOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalBasePWMap: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalBasePWMap: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::Set v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate Set ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate SetBinOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalBasePWMap: trying to evaluate LinearExp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalBasePWMap: trying to evaluate MDLExp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate LinearMap ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::PWLMap v) const 
{
  LIB::BasePWMap res;

  EvalBaseMap visit_map(env_);
  for (AST::Expr e : v.maps())
    res.emplace(boost::apply_visitor(visit_map, e));

  return LIB::BasePWMap(res); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate SBG ", v, "\n");
  return LIB::BasePWMap(); 
}

LIB::BasePWMap EvalBasePWMap::operator()(AST::DSBG v) const 
{
  Util::ERROR("EvalBasePWMap: trying to evaluate DSBG ", v, "\n");
  return LIB::BasePWMap(); 
}

} // namespace Eval

} // namespace SBG
