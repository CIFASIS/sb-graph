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

#include "eval/visitors/eval_sbg.hpp"

namespace SBG {

namespace Eval {

EvalSBG::EvalSBG(unsigned int nmbr_dims, const LIB::PWMapFact &fact, VarEnv &env)
  : nmbr_dims_(nmbr_dims), fact_(fact), env_(env) {}

LIB::SBG EvalSBG::operator()(AST::Natural v) const
{
  Util::ERROR("EvalSBG: trying to evaluate Natural ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::Rational v) const
{
  Util::ERROR("EvalSBG: trying to evaluate Rational ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::VariableName v) const
{
  Util::ERROR("EvalSBG: variable ", v, " is not a Map\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::UnaryOp v) const 
{
  Util::ERROR("EvalSBG: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::BinOp v) const 
{
  Util::ERROR("EvalSBG: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::Call v) const
{
  Util::ERROR("EvalSBG: trying to evaluate Call ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::Interval v) const
{ 
  Util::ERROR("EvalSBG: trying to evaluate Interval ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalSBG: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalSBG: trying to evaluate InterBinOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::MultiDimInter v) const
{ 
  Util::ERROR("EvalSBG: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalSBG: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalSBG: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::Set v) const 
{
  Util::ERROR("EvalSBG: trying to evaluate Set ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::SetUnaryOp v) const 
{
  Util::ERROR("EvalSBG: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::SetBinOp v) const 
{
  Util::ERROR("EvalSBG: trying to evaluate SetBinOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::LinearExp v) const
{ 
  Util::ERROR("EvalSBG: trying to evaluate LinearExp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::LExpBinOp v) const
{ 
  Util::ERROR("EvalSBG: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::MDLExp v) const
{ 
  Util::ERROR("EvalSBG: trying to evaluate MDLExp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::MDLExpBinOp v) const
{ 
  Util::ERROR("EvalSBG: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::LinearMap v) const
{ 
  Util::ERROR("EvalSBG: trying to evaluate LinearMap ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::PWLMap v) const
{ 
  Util::ERROR("EvalSBG: trying to evaluate PWLMap ", v, "\n");
  return LIB::SBG(fact_); 
}

LIB::SBG EvalSBG::operator()(AST::SBG v) const
{ 
  EvalSet visit_set(nmbr_dims_, fact_, env_);
  EvalPWMap visit_pw(nmbr_dims_, fact_, env_);

  LIB::Set V = boost::apply_visitor(visit_set, v.V());
  LIB::PWMap Vmap = boost::apply_visitor(visit_pw, v.Vmap());
  LIB::PWMap map1 = boost::apply_visitor(visit_pw, v.map1());
  LIB::PWMap map2 = boost::apply_visitor(visit_pw, v.map2());
  LIB::PWMap Emap = boost::apply_visitor(visit_pw, v.Emap());
  LIB::PWMap subE = boost::apply_visitor(visit_pw, v.subE_map());

  if (subE.dom().isEmpty() && !Emap.dom().isEmpty()) {
    unsigned int j = 1;
    for (const LIB::Map &m : Emap) {
      LIB::Set dom = m.dom();
      for (const LIB::SetPiece &mdi : dom) {
        LIB::Exp off(LIB::MD_NAT(mdi.arity(), j));
        subE.emplaceBack(fact_.createMap(fact_.createSet(mdi), off)); 
        ++j;
      }
    }
  } 

  return LIB::SBG(fact_, V, Vmap, map1, map2, Emap, subE);
}

LIB::SBG EvalSBG::operator()(AST::DSBG v) const
{ 
  Util::ERROR("EvalSBG: trying to evaluate DSBG ", v, "\n");
  return LIB::SBG(fact_); 
}

} // namespace Eval

} // namespace SBG
