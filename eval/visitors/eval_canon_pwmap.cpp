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

#include "eval/visitors/eval_canon_pwmap.hpp"

namespace SBG {

namespace Eval {

EvalCanonPWMap::EvalCanonPWMap() : env_() {}
EvalCanonPWMap::EvalCanonPWMap(VarEnv env) : env_(env) {}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::Natural v) const
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate Natural ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate MDNatural ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::Rational v) const
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate Rational ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate Boolean ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<MapBaseType>(value)) {
      auto m = std::get<MapBaseType>(value);
      if (std::holds_alternative<LIB::CanonPWMap>(m))
        return std::get<LIB::CanonPWMap>(m);
    }

    else {
      Util::ERROR("EvalCanonPWMap: variable ", v, " is not a PWMap\n");
      return LIB::CanonPWMap(); 
    } 
  }

  Util::ERROR("EvalCanonPWMap: variable ", v, " undefined\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::Call v) const
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate Call ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalCanonPWMap: trying to evaluate Interval ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate InterBinOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalCanonPWMap: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::Set v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate Set ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate SetBinOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalCanonPWMap: trying to evaluate LinearExp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalCanonPWMap: trying to evaluate MDLExp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate LinearMap ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::PWLMap v) const 
{
  LIB::CanonPWMap res;

  EvalCanonMap visit_map(env_);
  for (const AST::Expr &e : v.maps())
    res.emplace(boost::apply_visitor(visit_map, e));

  return res;
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate SBG ", v, "\n");
  return LIB::CanonPWMap(); 
}

LIB::CanonPWMap EvalCanonPWMap::operator()(AST::DSBG v) const 
{
  Util::ERROR("EvalCanonPWMap: trying to evaluate DSBG ", v, "\n");
  return LIB::CanonPWMap(); 
}

} // namespace Eval

} // namespace SBG
