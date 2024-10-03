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

#include "eval/visitors/eval_base_map.hpp"

namespace SBG {

namespace Eval {

EvalBaseMap::EvalBaseMap() : env_() {}
EvalBaseMap::EvalBaseMap(VarEnv env) : env_(env) {}

LIB::BaseMap EvalBaseMap::operator()(AST::Natural v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate Natural ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDNatural v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate MDNatural ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Rational v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate Rational ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Boolean v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate Boolean ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<MapBaseType>(value)) {
      auto m = std::get<MapBaseType>(value);
      if (std::holds_alternative<LIB::BaseMap>(m))
        return std::get<LIB::BaseMap>(m);
    }

    else {
      Util::ERROR("EvalBaseMap: variable ", v, " is not a linear expression\n");
      return LIB::BaseMap(); 
    } 
  }

  Util::ERROR("EvalBaseMap: variable ", v, " undefined");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Call v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate Call ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate Interval ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate InterBinOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalBaseMap: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::Set v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate Set ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate SetBinOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate LinearExp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalBaseMap: trying to evaluate MDLExp ", v, "\n");
  return LIB::BaseMap();
}

LIB::BaseMap EvalBaseMap::operator()(AST::MDLExpBinOp v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::LinearMap v) const 
{
  EvalUnordSet visit_set(env_);
  EvalMDLE visit_mdle(env_);

  return LIB::BaseMap(boost::apply_visitor(visit_set, v.dom())
                      , boost::apply_visitor(visit_mdle, v.lexp()));
}

LIB::BaseMap EvalBaseMap::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate PWLMap ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate SBG ", v, "\n");
  return LIB::BaseMap(); 
}

LIB::BaseMap EvalBaseMap::operator()(AST::DSBG v) const 
{
  Util::ERROR("EvalBaseMap: trying to evaluate DSBG ", v, "\n");
  return LIB::BaseMap(); 
}

} // namespace Eval

} // namespace SBG
