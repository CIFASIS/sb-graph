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

EvalGraphType::EvalGraphType() : sbg_(), dsbg_() {}
EvalGraphType::EvalGraphType(LIB::SBGraph sbg) : sbg_(sbg), dsbg_() {}
EvalGraphType::EvalGraphType(LIB::DSBGraph dsbg) : sbg_(), dsbg_(dsbg) {}

member_imp(EvalGraphType, LIB::SBGraph, sbg);
member_imp(EvalGraphType, LIB::DSBGraph, dsbg);

EvalGraph::EvalGraph() : nmbr_dims_(), env_() {}
EvalGraph::EvalGraph(unsigned int nmbr_dims, VarEnv env) : nmbr_dims_(nmbr_dims), env_(env) {}

EvalGraphType EvalGraph::operator()(AST::Natural v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a Natural");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::MDNatural v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a MDNatural");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::Rational v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a Rational");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::Boolean v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a Boolean");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(Util::VariableName v) const
{
  Util::ERROR("EvalGraph: variable %s not defined", v.c_str());
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::UnaryOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an arithmetic UnaryOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::BinOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an arithmetic BinOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::Call v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a Call");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::Interval v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an Interval");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::InterUnaryOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an InterUnaryOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::InterBinOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an InterBinOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::MultiDimInter v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an MultiDimInter");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::MDInterUnaryOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an MDInterUnaryOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::MDInterBinOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate an MDInterBinOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::Set v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a Set");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::SetUnaryOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a SetUnaryOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::SetBinOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a SetBinOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::LinearExp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a LinearExp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::LExpBinOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a LExpBinOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::MDLExp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a MDLExp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::MDLExpBinOp v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a MDLExpBinOp");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::LinearMap v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a LinearMap");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::PWLMap v) const
{
  Util::ERROR("EvalGraph: trying to evaluate a PWLMap");
  return EvalGraphType();
}

EvalGraphType EvalGraph::operator()(AST::SBG v) const
{
  EvalSet visit_set(nmbr_dims_, env_);
  EvalPWMap visit_pwmap(nmbr_dims_, env_);

  LIB::Set new_V = Apply(visit_set, v.V());
  LIB::PWMap new_Vmap = Apply(visit_pwmap, v.Vmap());

  LIB::PWMap new_map1 = Apply(visit_pwmap, v.map1());
  LIB::PWMap new_map2 = Apply(visit_pwmap, v.map2());
  LIB::PWMap new_Emap = Apply(visit_pwmap, v.Emap());

  return EvalGraphType(LIB::SBGraph(new_V, new_Vmap, new_map1, new_map2, new_Emap));
}

EvalGraphType EvalGraph::operator()(AST::DSBG v) const
{
  EvalSet visit_set(nmbr_dims_, env_);
  EvalPWMap visit_pwmap(nmbr_dims_, env_);

  LIB::Set new_V = Apply(visit_set, v.V());
  LIB::PWMap new_Vmap = Apply(visit_pwmap, v.Vmap());

  LIB::PWMap new_mapB = Apply(visit_pwmap, v.mapB());
  LIB::PWMap new_mapD = Apply(visit_pwmap, v.mapD());
  LIB::PWMap new_Emap = Apply(visit_pwmap, v.Emap());

  return EvalGraphType(LIB::SBGraph(new_V, new_Vmap, new_mapB, new_mapD, new_Emap));
}

} // namespace Eval

} // namespace SBG
