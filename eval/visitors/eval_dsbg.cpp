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

#include "eval/visitors/eval_dsbg.hpp"

namespace SBG {

namespace Eval {

EvalDSBG::EvalDSBG(unsigned int nmbr_dims, const LIB::PWMapAF &fact, VarEnv &env)
  : nmbr_dims_(nmbr_dims), fact_(fact), env_(env) {}

LIB::DSBG EvalDSBG::operator()(AST::Natural v) const
{
  Debug::ERROR("EvalDSBG: trying to evaluate Natural ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::Rational v) const
{
  Debug::ERROR("EvalDSBG: trying to evaluate Rational ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::VariableName v) const
{
  Debug::ERROR("EvalDSBG: variable ", v, " is not a Map\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::UnaryOp v) const 
{
  Debug::ERROR("EvalDSBG: trying to evaluate arithmetic UnaryOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::BinOp v) const 
{
  Debug::ERROR("EvalDSBG: trying to evaluate arithmetic BinOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::Call v) const
{
  Debug::ERROR("EvalDSBG: trying to evaluate Call ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::Interval v) const
{ 
  Debug::ERROR("EvalDSBG: trying to evaluate Interval ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::InterUnaryOp v) const
{
  Debug::ERROR("EvalDSBG: trying to evaluate InterUnaryOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::InterBinOp v) const
{
  Debug::ERROR("EvalDSBG: trying to evaluate InterBinOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::MultiDimInter v) const
{ 
  Debug::ERROR("EvalDSBG: trying to evaluate MultiDimInter ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::MDInterUnaryOp v) const
{
  Debug::ERROR("EvalDSBG: trying to evaluate MDInterUnaryOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::MDInterBinOp v) const
{
  Debug::ERROR("EvalDSBG: trying to evaluate MDInterBinOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::Set v) const 
{
  Debug::ERROR("EvalDSBG: trying to evaluate Set ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::SetUnaryOp v) const 
{
  Debug::ERROR("EvalDSBG: trying to evaluate SetUnaryOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::SetBinOp v) const 
{
  Debug::ERROR("EvalDSBG: trying to evaluate SetBinOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::LinearExp v) const
{ 
  Debug::ERROR("EvalDSBG: trying to evaluate LinearExp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::LExpBinOp v) const
{ 
  Debug::ERROR("EvalDSBG: trying to evaluate LExpBinOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::MDLExp v) const
{ 
  Debug::ERROR("EvalDSBG: trying to evaluate MDLExp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::MDLExpBinOp v) const
{ 
  Debug::ERROR("EvalDSBG: trying to evaluate MDLExpBinOp ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::LinearMap v) const
{ 
  Debug::ERROR("EvalDSBG: trying to evaluate LinearMap ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::PWLMap v) const
{ 
  Debug::ERROR("EvalDSBG: trying to evaluate PWLMap ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::SBG v) const
{ 
  Debug::ERROR("EvalDSBG: trying to evaluate SBG ", v, "\n");
  return LIB::DSBG(fact_); 
}

LIB::DSBG EvalDSBG::operator()(AST::DSBG v) const
{ 
  EvalSet visit_set(nmbr_dims_, fact_, env_);
  EvalPWMap visit_pw(nmbr_dims_, fact_, env_);

  LIB::Set V = boost::apply_visitor(visit_set, v.V());
  LIB::PWMap Vmap = boost::apply_visitor(visit_pw, v.Vmap());
  LIB::PWMap mapB = boost::apply_visitor(visit_pw, v.mapB());
  LIB::PWMap mapD = boost::apply_visitor(visit_pw, v.mapD());
  LIB::PWMap Emap = boost::apply_visitor(visit_pw, v.Emap());
  LIB::PWMap subE = boost::apply_visitor(visit_pw, v.subE_map());

  if (subE.dom().isEmpty() && !Emap.dom().isEmpty()) {
    unsigned int j = 1;
    for (const LIB::Map &sbgmap : Emap) {
      LIB::Set dom = sbgmap.dom();
      for (const LIB::SetPiece &mdi : dom) {
        LIB::Exp off(LIB::MD_NAT(mdi.arity(), j));
        subE.emplaceBack(fact_.createMap(fact_.createSet(mdi), off)); 
        ++j;
      }
    }
  } 

  return LIB::DSBG(fact_, V, Vmap, mapB, mapD, Emap, subE);
}

} // namespace Eval

} // namespace SBG
