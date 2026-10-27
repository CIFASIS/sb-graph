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

#include "algorithms/cc/cc.hpp"
#include "algorithms/cutvertex/cv_fact.hpp"
#include "algorithms/matching/bfs_matching.hpp"
#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/toposort/ts_fact.hpp"
#include "algorithms/misc/causalization_builders.hpp"
#include "algorithms/misc/causalization_json.hpp"
#include "eval/visitors/expr_evaluator.hpp"
#include "eval/visitors/func_evaluator.hpp"
#include "eval/visitors/linear_expr_evaluator.hpp"
#include "eval/visitors/nat_evaluator.hpp"
#include "eval/visitors/rational_evaluator.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Expression evaluator --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

template<typename T>
T eval(const ExprEvaluator &visit, AST::Expr e, std::string t = "UNDEF")
{
  ExprBaseType visited = boost::apply_visitor(visit, e);

  Util::ERROR_UNLESS(std::holds_alternative<T>(visited)
    , "ExprEvaluator: expression ", e, " is not of type ", t, "\n");

  return std::get<T>(visited);
}

ExprEvaluator::ExprEvaluator(EvalContext& eval_ctx) : eval_ctx_(eval_ctx)
{
  // Set built-in functions
  eval_ctx.insertFunction("isEmpty", BuiltInFunctions::emptyEvaluator);
  eval_ctx.insertFunction("minElem", BuiltInFunctions::minEvaluator);
  eval_ctx.insertFunction("maxElem", BuiltInFunctions::maxEvaluator);
  eval_ctx.insertFunction("restrict", BuiltInFunctions::restrictEvaluator);
  eval_ctx.insertFunction("compose", BuiltInFunctions::composeEvaluator);
  eval_ctx.insertFunction("inv", BuiltInFunctions::inverseEvaluator);
  eval_ctx.insertFunction("image", BuiltInFunctions::imageEvaluator);
  eval_ctx.insertFunction("preImage", BuiltInFunctions::preImageEvaluator);
  eval_ctx.insertFunction("dom", BuiltInFunctions::domEvaluator);
  eval_ctx.insertFunction("combine", BuiltInFunctions::combineEvaluator);
  eval_ctx.insertFunction("firstInv", BuiltInFunctions::firstInvEvaluator);
  eval_ctx.insertFunction("minMap", BuiltInFunctions::minMapEvaluator);
  eval_ctx.insertFunction("reduce", BuiltInFunctions::reduceEvaluator);
  eval_ctx.insertFunction("minAdj", BuiltInFunctions::minAdjEvaluator);
  eval_ctx.insertFunction("mapInf", BuiltInFunctions::mapInfEvaluator);
  eval_ctx.insertFunction("cc", BuiltInFunctions::connectedEvaluator);
  eval_ctx.insertFunction("match", BuiltInFunctions::matchingEvaluator);
  eval_ctx.insertFunction("scc", BuiltInFunctions::sccEvaluator);
  eval_ctx.insertFunction("sort", BuiltInFunctions::topoSortEvaluator);
  eval_ctx.insertFunction("cut", BuiltInFunctions::cutVertexEvaluator);
  eval_ctx.insertFunction("matchSCC", BuiltInFunctions::matchSCCEvaluator);
}

ExprBaseType ExprEvaluator::operator()(AST::Natural v) const
{
  return (LIB::NAT) v;
}

ExprBaseType ExprEvaluator::operator()(AST::Rational v) const
{
  return boost::apply_visitor(RationalEvaluator(eval_ctx_.venv())
    , AST::Expr(v));
}

ExprBaseType ExprEvaluator::operator()(AST::Name v) const 
{
  auto var_definition = eval_ctx_.venv().find(v);
  if (var_definition != eval_ctx_.venv().end()) { 
    return var_definition->second;
  }

  Util::ERROR("ExprEvaluator: variable ", v, " undefined\n");
  return ExprBaseType(); 
}

ExprBaseType ExprEvaluator::operator()(AST::UnaryOp v) const
{
  ExprBaseType inner = boost::apply_visitor(*this, v.expr());
  EBTList evaluated_args;
  evaluated_args.push_back(inner);
  UnaryOpEvaluator unary_op_eval;
  return unary_op_eval.evaluate(evaluated_args, v.op());
}

ExprBaseType ExprEvaluator::operator()(AST::BinOp v) const
{ 
  ExprBaseType left = boost::apply_visitor(*this, v.left());
  ExprBaseType right = boost::apply_visitor(*this, v.right());
  EBTList evaluated_args;
  evaluated_args.push_back(left);
  evaluated_args.push_back(right);
  BinOpEvaluator bin_op_eval;
  return bin_op_eval.evaluate(evaluated_args, v.op());
}

ExprBaseType ExprEvaluator::operator()(AST::Call v) const
{
  std::string func_name = v.name();

  auto func_definition = eval_ctx_.fenv().find(func_name);
  if (func_definition != eval_ctx_.fenv().end()) { 
    std::vector<ExprBaseType> evaluated_args;
    for (AST::Expr a : v.args()) {
      evaluated_args.push_back(boost::apply_visitor(*this, a));
    }

    return func_definition->second(evaluated_args);
  }

  Util::ERROR("ExprEvaluator: function ", func_name, " doesn't exist\n");
  return ExprBaseType();
}

ExprBaseType ExprEvaluator::operator()(AST::Interval v) const
{
  NatEvaluator visit_nat(eval_ctx_.venv());

  LIB::NAT b = boost::apply_visitor(visit_nat, v.begin());
  LIB::NAT s = boost::apply_visitor(visit_nat, v.step());
  LIB::NAT e = boost::apply_visitor(visit_nat, v.end());

  return LIB::Interval(b, s, e);
}

ExprBaseType ExprEvaluator::operator()(AST::MultiDimInter v) const
{
  LIB::SetPiece res;

  for (const AST::Expr &e : v.intervals()) 
    res.emplaceBack(eval<LIB::Interval>(*this, e, "Interval"));

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "EvalMDI[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}


ExprBaseType ExprEvaluator::operator()(AST::Set v) const
{
  LIB::Set res = LIB::SET_FACT.createSet();

  for (const AST::Expr &e : v.pieces())
    res.emplaceBack(eval<LIB::SetPiece>(*this, e, "SetPiece"));

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "ExprEvaluator[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType ExprEvaluator::operator()(AST::LinearExp v) const
{
  LinearExprEvaluator visit_le(eval_ctx_.venv());
  return boost::apply_visitor(visit_le, AST::Expr(v));
}

ExprBaseType ExprEvaluator::operator()(AST::MDLExp v) const
{
  LIB::Exp res;

  LinearExprEvaluator visit_le(eval_ctx_.venv());
  for (const AST::Expr &e : v.exps()) {
    LIB::LExp ith = boost::apply_visitor(visit_le, e);
    res.emplaceBack(ith);
  }

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "ExprEvaluator[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType ExprEvaluator::operator()(AST::LinearMap v) const
{
  LIB::Set d = eval<LIB::Set>(*this, v.dom(), "Set");
  LIB::Exp e = eval<LIB::Exp>(*this, v.lexp(), "Exp");

  LIB::Map res(d, e);

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "ExprEvaluator[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType ExprEvaluator::operator()(AST::PWLMap v) const
{
  LIB::PWMap res = LIB::PW_FACT.createPWMap();

  for (const AST::Expr &e : v.maps())
    res.emplaceBack(eval<LIB::Map>(*this, e, "Map"));

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "ExprEvaluator[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType ExprEvaluator::operator()(AST::SBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V(), "Set");
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap(), "PWMap");
  LIB::PWMap map1 = eval<LIB::PWMap>(*this, v.map1(), "PWMap");
  LIB::PWMap map2 = eval<LIB::PWMap>(*this, v.map2(), "PWMap");
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap(), "PWMap");
  LIB::PWMap subE = eval<LIB::PWMap>(*this, v.subE_map(), "PWMap");

  if (subE.dom().isEmpty() && !Emap.dom().isEmpty()) {
    unsigned int j = 1;
    for (const LIB::Map &m : Emap) {
      for (const LIB::SetPiece &mdi : m.dom()) {
        LIB::Exp off(LIB::MD_NAT(mdi.arity(), j));
        subE.emplaceBack(LIB::Map(LIB::SET_FACT.createSet(mdi), off)); 
        ++j;
      }
    }
  } 

  return LIB::SBG(V, Vmap, map1, map2, Emap, subE);
}

ExprBaseType ExprEvaluator::operator()(AST::BipartiteSBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V(), "Set");
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap(), "PWMap");
  LIB::PWMap map1 = eval<LIB::PWMap>(*this, v.map1(), "PWMap");
  LIB::PWMap map2 = eval<LIB::PWMap>(*this, v.map2(), "PWMap");
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap(), "PWMap");
  LIB::PWMap subE = eval<LIB::PWMap>(*this, v.subE_map(), "PWMap");
  LIB::Set X = eval<LIB::Set>(*this, v.X(), "Set");
  LIB::Set Y = eval<LIB::Set>(*this, v.Y(), "Set");

  if (subE.dom().isEmpty() && !Emap.dom().isEmpty()) {
    unsigned int j = 1;
    for (const LIB::Map &m : Emap) {
      for (const LIB::SetPiece &mdi : m.dom()) {
        LIB::Exp off(LIB::MD_NAT(mdi.arity(), j));
        subE.emplaceBack(LIB::Map(LIB::SET_FACT.createSet(mdi), off)); 
        ++j;
      }
    }
  } 

  return LIB::BipartiteSBG(V, Vmap, map1, map2, Emap, subE, X, Y);
}

ExprBaseType ExprEvaluator::operator()(AST::DSBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V(), "PWMap");
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap(), "PWMap");
  LIB::PWMap mapB = eval<LIB::PWMap>(*this, v.mapB(), "PWMap");
  LIB::PWMap mapD = eval<LIB::PWMap>(*this, v.mapD(), "PWMap");
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap(), "PWMap");
  LIB::PWMap subE = eval<LIB::PWMap>(*this, v.subE_map(), "PWMap");

  if (subE.dom().isEmpty() && !Emap.dom().isEmpty()) {
    unsigned int j = 1;
    for (const LIB::Map &m : Emap) {
      for (const LIB::SetPiece &mdi : m.dom()) {
        LIB::Exp off(LIB::MD_NAT(mdi.arity(), j));
        subE.emplaceBack(LIB::Map(LIB::SET_FACT.createSet(mdi), off)); 
        ++j;
      }
    }
  } 

  return LIB::DSBG(V, Vmap, mapB, mapD, Emap, subE);
}

ExprBaseType ExprEvaluator::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace Eval

} // namespace SBG
