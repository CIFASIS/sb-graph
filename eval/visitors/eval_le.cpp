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

#include "eval/visitors/eval_le.hpp"

namespace SBG {

namespace Eval {

EvalLE::EvalLE() : env_() {}
EvalLE::EvalLE(VarEnv env) : env_(env) {}

LIB::LExp EvalLE::operator()(AST::Natural v) const
{
  Util::ERROR("EvalLE: trying to evaluate Natural ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalLE: trying to evaluate MDNatural ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::Rational v) const
{
  Util::ERROR("EvalLE: trying to evaluate Rational ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalLE: trying to evaluate Boolean ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(Util::VariableName v) const
{
  MaybeEBT v_opt = env_[v];
  if (v_opt) { 
    ExprBaseType value = *v_opt;
    if (std::holds_alternative<LinearBaseType>(value)) {
      auto l = std::get<LinearBaseType>(value);
      if (std::holds_alternative<LIB::LExp>(l)) 
        return std::get<LIB::LExp>(l);
    }

    else {
      Util::ERROR("EvalLE: variable ", v, " is not a linear expression\n");
      return LIB::LExp(); 
    } 
  }

  Util::ERROR("EvalLE: variable ", v, " undefined\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::UnaryOp v) const
{
  Util::ERROR("EvalLE: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalLE: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::Call v) const
{
  Util::ERROR("EvalLE: trying to evaluate Call ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalLE: trying to evaluate Interval ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalLE: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalLE: trying to evaluate InterBinOp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalLE: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalLE: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalLE: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::Set v) const 
{
  Util::ERROR("EvalLE: trying to evaluate Set ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalLE: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalLE: trying to evaluate SetBinOp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::LinearExp v) const
{ 
  EvalRat visit_rat(env_);
  AST::Expr m = v.slope();
  AST::Expr h = v.offset();

  return LIB::LExp(boost::apply_visitor(visit_rat, m)
                   , boost::apply_visitor(visit_rat, h));
}

LIB::LExp EvalLE::operator()(AST::LExpBinOp v) const 
{
  LIB::LExp l = boost::apply_visitor(*this, v.left());
  LIB::LExp r = boost::apply_visitor(*this, v.right());
  switch (v.op()) {
    case AST::ExpOp::add:
      return l + r;

    case AST::ExpOp::sub:
      return l - r;

    default:
      Util::ERROR("EvalLE: LExpBinOp ", v.op(), " unsupported\n");
      return LIB::LExp(); 
  }
}

LIB::LExp EvalLE::operator()(AST::MDLExp v) const
{
  Util::ERROR("EvalLE: trying to evaluate MDLExp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::MDLExpBinOp v) const
{
  Util::ERROR("EvalLE: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::LinearMap v) const 
{
  Util::ERROR("EvalLE: trying to evaluate LinearMap ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::PWLMap v) const 
{
  Util::ERROR("EvalLE: trying to evaluate PWLMap ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::SBG v) const 
{
  Util::ERROR("EvalLE: trying to evaluate SBG ", v, "\n");
  return LIB::LExp(); 
}

LIB::LExp EvalLE::operator()(AST::DSBG v) const 
{
  Util::ERROR("EvalLE: trying to evaluate DSBG ", v, "\n");
  return LIB::LExp(); 
}


} // namespace Eval

} // namespace SBG
