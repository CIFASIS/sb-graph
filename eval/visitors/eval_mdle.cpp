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

#include "eval/visitors/eval_mdle.hpp"

namespace SBG {

namespace Eval {

EvalMDLE::EvalMDLE() : env_() {}
EvalMDLE::EvalMDLE(VarEnv env) : env_(env) {}

LIB::Exp EvalMDLE::operator()(AST::Natural v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate Natural ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate MDNatural ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::Rational v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate Rational ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate Boolean ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LinearBaseType>(value)) {
      auto l = std::get<LinearBaseType>(value);
      if (std::holds_alternative<LIB::Exp>(l))
        return std::get<LIB::Exp>(l);
    }

    else {
      Util::ERROR("EvalMDLE: variable ", v, " is not a mdle\n");
      return LIB::Exp(); 
    } 
  }

  Util::ERROR("EvalMDLE: variable ", v, " undefined\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::UnaryOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate arithmetic UnaryOp ", v , "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::Call v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate Call ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalMDLE: trying to evaluate Interval ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate InterBinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalMDLE: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::Set v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate Set ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate SetBinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalMDLE: trying to evaluate LinearExp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::LExpBinOp v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::MDLExp v) const
{ 
  LIB::Exp res;

  EvalLE visit_le(env_);
  for (AST::Expr le : v.exps())
    res.emplaceBack(boost::apply_visitor(visit_le, le));

  return LIB::Exp(res);
}

LIB::Exp EvalMDLE::operator()(AST::MDLExpBinOp v) const 
{
  LIB::Exp l = boost::apply_visitor(*this, v.left());
  LIB::Exp r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ExpOp::add:
      return l + r;

    case AST::ExpOp::sub:
      return l - r;

    default:
      Util::ERROR("EvalMDLE: LExpBinOp ", v.op(), " unsupported\n");
      return LIB::Exp(); 
  }
}

LIB::Exp EvalMDLE::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate LinearMap ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate PWLMap ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate SBG ", v, "\n");
  return LIB::Exp(); 
}

LIB::Exp EvalMDLE::operator()(AST::DSBG v) const 
{
  Util::ERROR("EvalMDLE: trying to evaluate DSBG ", v, "\n");
  return LIB::Exp(); 
}

} // namespace Eval

} // namespace SBG
